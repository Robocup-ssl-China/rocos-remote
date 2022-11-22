#include "interaction.h"
#include "visionmodule.h"
#include "parammanager.h"
#include <QtDebug>
Interaction::Interaction(QObject *parent) : QObject(parent){
    connect(&redraw_timer,SIGNAL(timeout()),this,SLOT(drawSignal()));
    redraw_timer.start(300);
}
Interaction::~Interaction(){
    redraw_timer.stop();
    disconnect(&redraw_timer, SIGNAL(timeout()), 0, 0);
}
void Interaction::setVision(bool needStart, bool real) {
    if (needStart) {
        VisionModule::instance()->udpSocketConnect(real);
    }
    else {
        VisionModule::instance()->udpSocketDisconnect();
    }
}
void Interaction::trigger4draw(){
    CInteraction::_()->_draw_signal.publish();
}
void Interaction::updateInterfaces(){
    NetworkInterfaceManager::instance()->updateInterfaces();
}
bool Interaction::setInterfaceIndex(const int index){
    return NetworkInterfaceManager::_()->set("vision",index);
}
bool Interaction::controlCore(bool main,bool control){
    QProcess*& process = (main ? CInteraction::_()->mainCore : CInteraction::_()->oppCore);
    CInteraction::_()->stopProcess(process);
    if(control){
        process = new QProcess();
        process->setProgram("./TinyCore");
        process->start();
    }
    return true;
}
void Interaction::changeCoreSetting(bool ifYellow,bool ifRight){
    ZSS::ZPM::instance()->changeParam("ZAlert/IsYellow", ifYellow);
    ZSS::ZPM::instance()->changeParam("ZAlert/IsRight", ifRight);
}
QStringList Interaction::getInterfaces(){
    return NetworkInterfaceManager::instance()->getInterfaces();
}
bool Interaction::setParam(QString key,QVariant value){
    return ZSS::ZPM::instance()->changeParam(key,value);
}
QVariant Interaction::getParam(QString key,QVariant defaultValue){
    return ZSS::ZPM::_()->value(key,defaultValue);
}
void Interaction::drawSignal(){
    CInteraction::_()->_extra_draw_signal.publish();
}
/*
 * CInteraction
 */
void CInteraction::stopProcess(QProcess*& process){
    if(process != nullptr && process->isOpen()){
        process->close();
    }
    delete process;
    process = nullptr;
}
CInteraction::CInteraction():_draw_signal("need_draw"),_extra_draw_signal("extra_need_draw"){
    killProcess("TinyCore");
    runUDPContext();
}
CInteraction::~CInteraction(){
    stopProcess(mainCore);
    stopProcess(oppCore);
    killProcess("TinyCore");

}
void CInteraction::killProcess(const QString& name){
#ifdef WIN32
    auto cmd = "taskkill -im " + name + ".exe -f";
#else
    auto cmd = "pkill " + name;
#endif
    QProcess::execute(cmd);
}
void CInteraction::runUDPContext(){
    if(running) {
        return;
    }
    _asio_context = std::thread([this]{
        running = true;
        zos::__io::GetInstance()->run();
        zos::__io::GetInstance()->reset();
        running = false;
    });
    _asio_context.detach();
}
