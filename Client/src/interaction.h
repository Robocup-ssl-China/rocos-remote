#ifndef INTERACTION_H
#define INTERACTION_H

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QTimer>
#include <QtDebug>
#include "qsingleton.h"
#include "networkinterfacemanager.h"
#include "zos/zos.h"
#include "zos/socket.h"
class Interaction : public QObject{
    Q_OBJECT
public:
    explicit Interaction(QObject *parent = nullptr);
    virtual ~Interaction();
    Q_INVOKABLE void setVision(bool, bool);
    Q_INVOKABLE void trigger4draw();
    Q_INVOKABLE QStringList getInterfaces();
    Q_INVOKABLE bool setInterfaceIndex(const int);
    Q_INVOKABLE void updateInterfaces();
    Q_INVOKABLE bool controlCore(bool,bool);
    Q_INVOKABLE void changeCoreSetting(bool,bool);
    Q_INVOKABLE bool setParam(QString,QVariant);
    Q_INVOKABLE QVariant getParam(QString,QVariant defaultValue=QVariant());
public slots:
    Q_INVOKABLE void drawSignal();
private:
    QTimer redraw_timer;
};

class CInteraction : public Singleton<CInteraction>{
public:
    CInteraction();
    ~CInteraction();
    void runUDPContext();
    zos::Publisher _draw_signal;
    zos::Publisher _extra_draw_signal;
    std::thread _asio_context;
public:
    void killProcess(const QString& name);
    void stopProcess(QProcess*&);
    QProcess *mainCore = nullptr;
    QProcess *oppCore = nullptr;
private:
    bool running = false;
};

#endif // INTERACTION_H
