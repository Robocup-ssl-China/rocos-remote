#include "networkinterfacemanager.h"
/*
 * Network Interface Manager
*/
NetworkInterfaceManager::NetworkInterfaceManager(){
}
void NetworkInterfaceManager::updateInterfaces(){
    this->interfaces.clear();
    for(auto interface : QNetworkInterface::allInterfaces()){
        interfaces.append(interface.humanReadableName());
    }
}
// bool NetworkInterfaceManager::set(const int index){
//     if(index >= 0 && index <interfaces.length()){
//         this->index = index;
//         return true;
//     }
//     this->index = -1;
//     return false;
// }
bool NetworkInterfaceManager::set(const std::string& name,const int index){
    indexes[name] = index;
    return true;
}
bool NetworkInterfaceManager::legal(const std::string& name){
    // check if exist
    if(indexes.find(name) == indexes.end()){
        return false;
    }
    // check value
    if(indexes[name] < 0 || indexes[name] >= interfaces.length()){
        return false;
    }
    return true;
}
QNetworkInterface NetworkInterfaceManager::get(const std::string& name){
    if(legal(name)){
        return QNetworkInterface::interfaceFromName(interfaces[indexes[name]]);
    }
    return QNetworkInterface();
}