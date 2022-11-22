#ifndef __NETWORKINTERFACEMANAGER_H__
#define __NETWORKINTERFACEMANAGER_H__
#include <QNetworkInterface>
#include "zos/zos.h"
class NetworkInterfaceManager:public Singleton<NetworkInterfaceManager>{
public:
    NetworkInterfaceManager();
    void updateInterfaces();
    QStringList getInterfaces(){ return interfaces; }
    bool set(const std::string& name,const int);
    bool legal(const std::string& name);
    QNetworkInterface get(const std::string&);
private:
    std::map<std::string,int> indexes;
    QStringList interfaces;
};
using NIM = NetworkInterfaceManager;
#endif // __NETWORKINTERFACEMANAGER_H__
