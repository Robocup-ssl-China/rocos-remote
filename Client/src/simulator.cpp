#include "simulator.h"
#include "sim/sslworld.h"
#include "staticparams.h"
#include <QHostAddress>
#include <QtDebug>
namespace{
//double xIn[PARAM::ROBOTNUM] = { 0.3,  0.6,  0.9,  1.2, 1.5, 1.8, 2.1, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.2, 4.5, 4.8};
//double yOut[PARAM::ROBOTNUM] = { -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4, -5.4 };
//double xOut[PARAM::ROBOTNUM] = { 0.4,  0.8,  1.2,  1.6,  2.0,  2.4, 2.8, 3.2, 3.6, 4.0, 4.4, 4.8 };
//double yIn[PARAM::ROBOTNUM] = { -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0, -4.0 };
}
Simulator::Simulator():p_sim_packet("grsim_packet"){
    ZSS::ZPM::instance()->loadParam(remote_sim, "Alert/RemoteSim", false);
    ZSS::ZPM::instance()->loadParam(remote_sim_address, "Alert/RemoteSimAddress", "127.0.0.1");
    p_sim_packet.link(&SSLWorld::instance()->s_sim_packet);
}
void Simulator::setBall(double x,double y,double vx,double vy){
    grSim_Packet packet;
    auto* replacement = packet.mutable_replacement();
    auto* ball = replacement->mutable_ball();
    ball->set_x(x);
    ball->set_y(y);
    ball->set_vx(vx);
    ball->set_vy(vy);
    send(&packet);
}
void Simulator::setRobot(double x,double y,int id,bool team,double dir,bool turnon){
    grSim_Packet packet;
    auto* replacement = packet.mutable_replacement();
    auto* robot = replacement->add_robots();
    robot->set_x(x);
    robot->set_y(y);
    robot->set_id(id);
    robot->set_dir(dir);
    robot->set_yellowteam(team);
    robot->set_turnon(turnon);
    send(&packet);
}
void Simulator::controlRobot(int num,bool team){
    grSim_Packet packet;
    double dir = (team ? 180 : 0);
    double ratio = (team ? 1 : -1);
    auto* replacement = packet.mutable_replacement();
    for(int i=0;i<num;i++){
        auto* robot = replacement->add_robots();
        robot->set_x(ratio*(i+1)*0.3);
        robot->set_y(0);
        robot->set_id(i);
        robot->set_dir(dir);
        robot->set_yellowteam(team);
        robot->set_turnon(true);
    }
    for(int i=num;i<PARAM::ROBOTMAXID;i++){
        auto* robot = replacement->add_robots();
        robot->set_x(ratio*i*0.3);
        robot->set_y(-5.5);
        robot->set_id(i);
        robot->set_dir(dir);
        robot->set_yellowteam(team);
        robot->set_turnon(false);
    }
    send(&packet);
}
void Simulator::send(grSim_Packet* packet){
    static zos::Data data;
    int size = packet->ByteSize();
    data.resize(size);
    packet->SerializeToArray(data.ptr(), size);
    if(!remote_sim){
        p_sim_packet.publish(data);
    }else{
        sendSocket.writeDatagram(static_cast<const char*>(data.data()),data.size(),QHostAddress(remote_sim_address),ZSS::Athena::SIM_SEND);
    }
}
