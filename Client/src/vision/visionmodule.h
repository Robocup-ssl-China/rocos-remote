#ifndef CVISIONMODULE_H
#define CVISIONMODULE_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QStringList>
#include <shared_mutex>
#include <staticparams.h>
#include "vision_detection.pb.h"
#include "messageformat.h"
#include "zos/utils/singleton.h"
#include "dataqueue.hpp"
#include "zos/core.h"
#include "zos/socketplugin.h"
#include "zss_cmd.pb.h"
#include "grSim_Packet.pb.h"

class VisionModule : public QObject,public Singleton<VisionModule>{
    Q_OBJECT
public:
    VisionModule();
    ~VisionModule();
    // UDP
    void udpSocketConnect(bool);
    void udpSocketDisconnect();

    // vision parse
    void parse(void*, int);
    bool collectNewVision();
    void dealWithData();

    // vision send
    void sendData();

    double calculateWeight(int camID, CGeoPoint);

    DataQueue<VisionMessage>_camera[PARAM::CAMERA];
    DataQueue<VisionMessage> _maintain;

    bool _camera_update[PARAM::CAMERA];
    zos::Publisher p_draw_signal;
public slots:
    // vision parse
    void storeData();
    void oneStepSimData();

private:
    // UDP
    QUdpSocket udpReceiveSocket;
    zos::udp::Plugin<Vision_DetectionFrame> udpSender;
    zos::udp::Plugin<ZSS::Protocol::Robots_Command>* commandReceiverYellow;
    zos::udp::Plugin<ZSS::Protocol::Robots_Command>* commandReceiverBlue;
    int vision_port;

    zos::Publisher p_sim_signal;
    zos::Publisher p_sim_packet;
    zos::Subscriber<10> s_ssl_vision;

    // vision parse
    bool _camera_control[PARAM::CAMERA];
    Vision_DetectionFrame _detectionFrame;
    std::shared_mutex _visionmutex;
    void resetVision();

    // utils
    void sendSimYellow(const void*, size_t);
    void sendSimBlue(const void*, size_t);
    void resetSimCommand();
    std::shared_mutex _commandmutex;
    void sendSim(int t, ZSS::Protocol::Robots_Command& command);
    void updateEdge();
    CameraEdge _anchor[PARAM::CAMERA];
    quint64 _count;
    QTimer _sim_timer;

    bool _running = false;

    // command
    grSim_Packet grsim_packet[PARAM::TEAMS];
    grSim_Commands *grsim_commands[PARAM::TEAMS];
    grSim_Robot_Command *grsim_robots[PARAM::TEAMS][PARAM::ROBOTNUM];

public:
    struct CheckRobotResult{
        bool res;
        int team;
        unsigned int id;
        double x,y,orientation;
    };
    CheckRobotResult checkRobot(double x,double y);
};

#endif // CVISIONMODULE_H
