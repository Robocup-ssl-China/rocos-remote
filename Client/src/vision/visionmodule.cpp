#include <QNetworkInterface>
#include <thread>
#include "messages_robocup_ssl_wrapper.pb.h"
#include "visionmodule.h"
#include "dealball.h"
#include "dealrobot.h"
#include "parammanager.h"
#include "field.h"
#include "staticparams.h"
#include "interaction.h"
#include "sim/sslworld.h"
#include "networkinterfacemanager.h"

VisionModule::VisionModule()
    : p_draw_signal("draw_signal")
    , p_sim_signal("sim_signal")
    , p_sim_packet("sim_packet")
    , s_ssl_vision("ssl_vision", [this](const zos::Data& data){parse((void*) data.data(), data.size());})
    , _running(false){
    std::fill_n(_camera_update, PARAM::CAMERA, false);
    std::fill_n(_camera_control, PARAM::CAMERA, false);
    _camera_control[0] = true;
    // test
    VisionMessage origin;
    _maintain.push(origin);
    SSLWorld::instance()->p_ssl_vision.link(&s_ssl_vision);
    p_sim_signal.link(&SSLWorld::instance()->s_sim_signal);
    p_sim_packet.link(&SSLWorld::instance()->s_sim_packet);
    commandReceiverYellow = commandReceiverBlue = nullptr;
}

void VisionModule::resetSimCommand() {
    QByteArray data;
    _commandmutex.lock();
    grsim_packet[0].Clear();
    grsim_packet[1].Clear();
    for (int i=0; i<PARAM::TEAMS; i++) {
        grsim_commands[i] = grsim_packet[i].mutable_commands();
        grsim_commands[i]->set_timestamp(0);
        grsim_commands[i]->set_isteamyellow(i);
        for (int j = 0; j < PARAM::ROBOTNUM; j++) {
            grsim_robots[i][j] = grsim_commands[i]->add_robot_commands();
            grsim_robots[i][j]->set_id(i);
            grsim_robots[i][j]->set_kickspeedx(0);
            grsim_robots[i][j]->set_kickspeedz(0);
            grsim_robots[i][j]->set_velnormal(0);
            grsim_robots[i][j]->set_veltangent(0);
            grsim_robots[i][j]->set_velangular(0);
            grsim_robots[i][j]->set_spinner(false);
            grsim_robots[i][j]->set_wheelsspeed(false);
        }
        int size = grsim_packet[i].ByteSize();
        data.resize(size);
        grsim_packet[i].SerializeToArray(data.data(), size);
        p_sim_packet.publish(data.data(), size);
    }
    _commandmutex.unlock();
}

VisionModule::~VisionModule()
{
    delete  commandReceiverBlue;
    delete  commandReceiverYellow;
}

void VisionModule::sendSim(int t, ZSS::Protocol::Robots_Command &command) {
    _commandmutex.lock();
    QByteArray data;
    assert (t>=0&&t<=1);
    int command_size = command.command_size();
    for (int i = 0; i < command_size; i++) {
        auto commands = command.command(i);
        auto id = commands.robot_id();
        grsim_robots[t][id]->set_id(id);
        grsim_robots[t][id]->set_wheelsspeed(false);
        //set flatkick or chipkick
        if (!commands.kick()) {
            grsim_robots[t][id]->set_kickspeedz(0);
            grsim_robots[t][id]->set_kickspeedx(commands.power() / 1000.);
        }
        else {
            double radian = ZSS::Sim::CHIP_ANGLE * ZSS::Sim::PI / 180.0;
            double vx = sqrt((commands.power() / 1000.) * ZSS::Sim::G / 2.0 / tan(radian));
            double vz = vx * tan(radian);
            grsim_robots[t][id]->set_kickspeedz(vx);
            grsim_robots[t][id]->set_kickspeedx(vz);
        }
        //set velocity and dribble
        double vx = commands.velocity_x();
        double vy = commands.velocity_y();
        double vr = commands.velocity_r();
        double dt = 1. / ZSS::Athena::FRAME_RATE;
        double theta = - vr * dt;
        CVector v(vx, vy);
        v = v.rotate(theta);
        if (fabs(theta) > 0.00001) {
            //            if (i==0) cout << theta << " " <<vx << " "<< vy << " ";
            v = v * theta / (2 * sin(theta / 2));
            vx = v.x();
            vy = v.y();
            //            if (i==0) cout << vx << " "<< vy << " " << endl;
        }

        grsim_robots[t][id]->set_veltangent(vx / 1000.);
        grsim_robots[t][id]->set_velnormal(vy / 1000.);
        grsim_robots[t][id]->set_velangular(vr);
        grsim_robots[t][id]->set_spinner(commands.dribbler_spin() > 1);
    }
    int size = grsim_packet[t].ByteSize();
    data.resize(size);
    grsim_packet[t].SerializeToArray(data.data(), size);
    p_sim_packet.publish(data.data(), size);
    for (int i = 0; i < PARAM::ROBOTNUM; i++) {
        grsim_robots[t][i]->set_id(i);
        grsim_robots[t][i]->set_kickspeedx(0);
        grsim_robots[t][i]->set_kickspeedz(0);
        grsim_robots[t][i]->set_velnormal(0);
        grsim_robots[t][i]->set_veltangent(0);
        grsim_robots[t][i]->set_velangular(0);
        grsim_robots[t][i]->set_spinner(false);
        grsim_robots[t][i]->set_wheelsspeed(false);
    }
    _commandmutex.unlock();
}

bool VisionModule::collectNewVision() {
    for (int i = 0; i < PARAM::CAMERA; i++) {
        if (_camera_control[i] && !_camera_update[i])
            return false;
    }
    return true;
}

void VisionModule::dealWithData() {
    _count ++;
    updateEdge();
    DealBall::instance()->run();
    DealRobot::instance()->run();

    // update ball and robot
    VisionMessage current;
    if (DealBall::instance()->result.valid)
        current.addBall(DealBall::instance()->result);
    for (int color = 0; color < PARAM::TEAMS; color++)
        for (int i = 0; i < PARAM::ROBOTNUM; i++) {
            if (DealRobot::instance()->finalSequence[color][i].valid){
                Robot robot = DealRobot::instance()->finalSequence[color][i];
                current.addRobot(color, i, robot.pos.x(), robot.pos.y(), robot.angle);
            }
        }
    _maintain.push(current);
    sendData();
    std::fill_n(_camera_update, PARAM::CAMERA, false);
}

void VisionModule::sendData() {
    if (!_running) return;
    std::shared_lock<std::shared_mutex> lock(_visionmutex);
    _detectionFrame.Clear();
    auto detectionBall = _detectionFrame.mutable_balls();
    VisionMessage result = _maintain[0];
    if (result.ballNum > 0) {
        detectionBall->set_x(result.ball[0].pos.x());
        detectionBall->set_y(result.ball[0].pos.y());
        detectionBall->set_vel_x(result.ball[0].vel.x());
        detectionBall->set_vel_y(result.ball[0].vel.y());
        detectionBall->set_valid(result.ball[0].valid);
        // raw
        detectionBall->set_raw_x(result.ball[0].pos.x());
        detectionBall->set_raw_y(result.ball[0].pos.y());
    } else {
        detectionBall->set_valid(false);
        detectionBall->set_x(-32767);
        detectionBall->set_y(-32767);
        detectionBall->set_raw_x(-32767);
        detectionBall->set_raw_y(-32767);
    }
    for (int team = 0; team < PARAM::TEAMS; team++) {
        for (int i = 0; i < result.robotNum[team]; i++) {
            Vision_DetectionRobot* robot;
            if (team == 0 )  robot = _detectionFrame.add_robots_blue();
            else robot = _detectionFrame.add_robots_yellow();
            robot->set_x(result.robot[team][i].pos.x());
            robot->set_y(result.robot[team][i].pos.y());
            robot->set_orientation(result.robot[team][i].angle);
            robot->set_robot_id(result.robot[team][i].id);
            robot->set_vel_x(result.robot[team][i].vel.x());
            robot->set_vel_y(result.robot[team][i].vel.y());
            robot->set_rotate_vel(result.robot[team][i].rotvel);
            robot->set_valid(true);

            // raw
            robot->set_raw_x(result.robot[team][i].pos.x());
            robot->set_raw_y(result.robot[team][i].pos.y());
        }
    }
    QByteArray data;
    int size = _detectionFrame.ByteSize();
    data.resize(size);
    _detectionFrame.SerializeToArray(data.data(),size);
    p_draw_signal.publish(data.data(),size);
    udpSender.sendData(data.data(),size,ZSS::Athena::VISION_SEND[PARAM::BLUE]);
    udpSender.sendData(data.data(),size,ZSS::Athena::VISION_SEND[PARAM::YELLOW]);
}

void VisionModule::updateEdge() {
    for (int camID = 0; camID < PARAM::CAMERA; camID++) {
        if (_camera_update[camID]) {
            for (int i = 0; i < _camera[camID][0].ballNum; i++) {
                _anchor[camID].update(_camera[camID][0].ball[i].pos, 1);
            }
            for (int color = 0; color < PARAM::TEAMS; color++){
                for (int i = 0; i < _camera[camID][0].robotNum[color]; i++) {
                    _anchor[camID].update(_camera[camID][0].robot[color][i].pos, 0);
                }
            }
        }
    }
}

double VisionModule::calculateWeight(int camID, CGeoPoint pos) {
    return 1.0;
}

void VisionModule::udpSocketDisconnect() {
    bool isSim;
    ZSS::ZPM::instance()->loadParam(isSim, "Alert/IsSimulation", false);
    _running = false;
    if (!isSim) {
        disconnect(&udpReceiveSocket);
        udpReceiveSocket.abort();
    }
    else {
        _sim_timer.stop();
        disconnect(&_sim_timer, SIGNAL(timeout()), 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        resetVision();
        resetSimCommand();
    }
}

void VisionModule::sendSimBlue(const void *ptr, size_t size) {
    if (!_running) return;
    ZSS::Protocol::Robots_Command commands;
    commands.ParseFromArray(ptr, size);
    sendSim(PARAM::BLUE, commands);
}

void VisionModule::sendSimYellow(const void *ptr, size_t size) {
    if (!_running) return;
    ZSS::Protocol::Robots_Command commands;
    commands.ParseFromArray(ptr, size);
    sendSim(PARAM::YELLOW, commands);
}

void VisionModule::resetVision() {
    qDebug() << "reset vision";
    std::scoped_lock<std::shared_mutex> lock(_visionmutex);
    _detectionFrame.Clear();
    QByteArray data;
    auto detectionBall = _detectionFrame.mutable_balls();
    detectionBall->set_valid(false);
    detectionBall->set_x(-32767);
    detectionBall->set_y(-32767);
    detectionBall->set_raw_x(-32767);
    detectionBall->set_raw_y(-32767);
    int size = _detectionFrame.ByteSize();
    data.resize(size);
    _detectionFrame.SerializeToArray(data.data(),size);
    p_draw_signal.publish(data.data(),size);
}

void VisionModule::udpSocketConnect(bool real) {
    ZSS::ZPM::instance()->changeParam("Alert/IsSimulation", !real);
    if(real) {
        ZSS::ZPM::instance()->loadParam(vision_port, "AlertPorts/Vision4Real", 10005);
        auto _interface = NIM::_()->get("vision");
        qDebug() << "VisionPort: " << vision_port;
        udpReceiveSocket.bind(QHostAddress::AnyIPv4, vision_port, QUdpSocket::ShareAddress);
        qDebug() << "Vision Interface: " << _interface.humanReadableName();
        udpReceiveSocket.joinMulticastGroup(QHostAddress(ZSS::SSL_ADDRESS), _interface);
        connect(&udpReceiveSocket, SIGNAL(readyRead()), this, SLOT(storeData()), Qt::DirectConnection);
    }
    else {
        std::fill_n(_camera_control, PARAM::CAMERA, false);
        _camera_control[0] = true;
        connect(&_sim_timer, SIGNAL(timeout()), this, SLOT(oneStepSimData()), Qt::DirectConnection);
        _sim_timer.start(14);
//        qDebug() << "resetSimCommand";
        resetSimCommand();
//        qDebug() << "resetSimCommand";
        if (commandReceiverYellow == nullptr) {
            commandReceiverYellow = new zos::udp::Plugin<ZSS::Protocol::Robots_Command>(ZSS::Athena::CONTROL_SEND[1], [this](const void* ptr, size_t size){sendSimYellow(ptr, size);});
        }
        if (commandReceiverBlue == nullptr) {
            commandReceiverBlue = new zos::udp::Plugin<ZSS::Protocol::Robots_Command>(ZSS::Athena::CONTROL_SEND[0], [this](const void* ptr, size_t size){sendSimBlue(ptr, size);});
        }
        CInteraction::instance()->runUDPContext();
    }
    _running = true;
}

void VisionModule::storeData() {
    static QByteArray datagram;
    while (udpReceiveSocket.hasPendingDatagrams()) {
        datagram.resize(udpReceiveSocket.pendingDatagramSize());
        udpReceiveSocket.readDatagram(datagram.data(), datagram.size());
        parse((void*)datagram.data(), datagram.size());
    }
}

void VisionModule::oneStepSimData() {
    p_sim_signal.publish("sim_signal");
}

void VisionModule::parse(void *ptr, int size) {
    static SSL_WrapperPacket packet;
    VisionMessage message;
    packet.ParseFromArray(ptr, size);
    if (packet.has_detection()) {
        const SSL_DetectionFrame& detection = packet.detection();
        if (detection.camera_id() >= PARAM::CAMERA || detection.camera_id() < 0) {
            return;
        }
        int ballSize = detection.balls_size();
        int blueSize = detection.robots_blue_size();
        int yellowSize = detection.robots_yellow_size();
//        if (DEBUG) {
//            std::cout << "receive from" << detection.camera_id() << " ballNum: " << ballSize << " blueNum: " << blueSize << " yellowNum: " << yellowSize << std::endl;
//        }
        // add ball
        for (int i = 0; i < ballSize; i++) {
            const SSL_DetectionBall& ball = detection.balls(i);
            if (true) {
                message.addBall(ball.x(), ball.y());
            }
        }
        // add blue robot
        for (int i = 0; i < blueSize; i++) {
            const SSL_DetectionRobot& robot = detection.robots_blue(i);
            if (true) {
                message.addRobot(PARAM::BLUE, robot.robot_id(), robot.x(), robot.y(), robot.orientation());
            }
        }
        // add yellow robot
        for (int i = 0; i < yellowSize; i++) {
            const SSL_DetectionRobot& robot = detection.robots_yellow(i);
            if (true) {
                message.addRobot(PARAM::YELLOW, robot.robot_id(), robot.x(), robot.y(), robot.orientation());
            }
        }

        _camera[detection.camera_id()].push(message);
        _camera_update[detection.camera_id()] = true;

        if (collectNewVision()) {
            dealWithData();
        }
    }
}

VisionModule::CheckRobotResult VisionModule::checkRobot(double x,double y){
    std::shared_lock lock(_visionmutex);
    int blue_size = _detectionFrame.robots_blue_size();
    int yellow_size = _detectionFrame.robots_yellow_size();
    for(int i=0;i<blue_size;i++){
        auto&& robot = _detectionFrame.robots_blue(i);
        if(auto dist = std::hypot(robot.x()-x,robot.y()-y);dist < PARAM::Vehicle::V2::PLAYER_SIZE){
            return {true,PARAM::BLUE,robot.robot_id(),robot.x(),robot.y(),robot.orientation()};
        }
    }
    for(int i=0;i<yellow_size;i++){
        auto&& robot = _detectionFrame.robots_yellow(i);
        if(auto dist = std::hypot(robot.x()-x,robot.y()-y);dist < PARAM::Vehicle::V2::PLAYER_SIZE){
            return {true,PARAM::YELLOW,robot.robot_id(),robot.x(),robot.y(),robot.orientation()};
        }
    }
    return {false,0,0,0,0,0};
}
