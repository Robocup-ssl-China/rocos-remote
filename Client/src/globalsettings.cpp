#include "globalsettings.h"
#include "field.h"
#include "parammanager.h"
CGlobalSettings::CGlobalSettings(QObject *parent)
    :QObject(parent)
    ,minimumX(-999999)
    ,maximumX(999999)
    ,minimumY(-999999)
    ,maximumY(999999)
    ,selectCarMinX(0)
    ,selectCarMaxX(0)
    ,selectCarMinY(0)
    ,selectCarMaxY(0)
    ,oldLogFlag(1){

}
bool CGlobalSettings::inChosenArea(float x, float y){
    return (x >= minimumX && x <= maximumX && y >= minimumY && y <= maximumY);
}
bool CGlobalSettings::inChosenArea(CGeoPoint point){
    return (point.x() >= minimumX && point.x() <= maximumX && point.y() >= minimumY && point.y() <= maximumY);
}
void CGlobalSettings::setArea(float a,float b,float c,float d){
    minimumX = a;
    minimumY = c;
    maximumX = b;
    maximumY = d;
}
void CGlobalSettings::resetArea(){
    int width,height;
    ZSS::ZPM::_()->loadParam(width,"field/canvasWidth",13200);
    ZSS::ZPM::_()->loadParam(height,"field/canvasHeight",9900);
    minimumX = -width/2;
    minimumY = -height/2;
    maximumX = width/2;
    maximumY = height/2;
}
void CGlobalSettings::setBallPlacementPos(float x,float y){
    ballPlacementX = x;
    ballPlacementY = y;
}
void CGlobalSettings::setSelectCarArea(float a,float b,float c,float d) {
    selectCarMinX = a;
    selectCarMinY = c;
    selectCarMaxX = b;
    selectCarMaxY = d;
}
void CGlobalSettings::resetSelectCarArea() {
    int width,height;
    ZSS::ZPM::_()->loadParam(width,"field/canvasWidth",13200);
    ZSS::ZPM::_()->loadParam(height,"field/canvasHeight",9900);
    selectCarMinX = 0;
    selectCarMaxX = 0;
    selectCarMinY = 0;
    selectCarMaxY = 0;
}
//void CGlobalSettings::setOldLogFlag(bool old) {
//    oldLogFlag = old ? -1 : 1;
//}
