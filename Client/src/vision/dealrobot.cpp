#include "dealrobot.h"
#include "visionmodule.h"

CDealRobot::CDealRobot()
{

}

void CDealRobot::run() {
    init();
    mergeRobot();
}

void CDealRobot::mergeRobot() {
    for (int color = 0; color < PARAM::TEAMS; color++) {
        for (int robotID = 0; robotID < PARAM::ROBOTNUM; robotID++) {
            bool isFound = false;
            double weight_total = 0.;
            CGeoPoint pos(0., 0.);
            double angle = 0.;
            for (int camID = 0; camID < PARAM::CAMERA; camID++) {
                if (robotSequence[color][robotID][camID].valid) {
                    isFound = true;
                    double weight = VisionModule::instance()->calculateWeight(camID, robotSequence[color][robotID][camID].pos);
                    pos.setX((weight_total * pos.x() + weight * robotSequence[color][robotID][camID].pos.x())/(weight_total + weight));
                    pos.setY((weight_total * pos.y() + weight * robotSequence[color][robotID][camID].pos.y())/(weight_total + weight));
                    angle = (angle * weight_total + robotSequence[color][robotID][camID].angle * weight) / (weight_total + weight);
                    weight_total += weight;
                }
            }
            if (isFound) {
                finalSequence[color][robotID].fill(robotID, pos.x(), pos.y(), angle);
            }
        }
    }
}



void CDealRobot::init() {
    for (int robotId = 0; robotId < PARAM::ROBOTNUM; robotId++) {
        for (int color = 0; color < PARAM::TEAMS; color++) {
            finalSequence[color][robotId].valid = false;
            for (int camID = 0; camID < PARAM::CAMERA; camID ++) {
                robotSequence[color][robotId][camID].valid = false;
            }
        }
    }
    for (int robotId = 0; robotId < PARAM::ROBOTNUM; robotId++) {
        for (int color = 0; color < PARAM::TEAMS; color++) {
            for (int camID = 0; camID < PARAM::CAMERA; camID ++){
                if (VisionModule::instance()->_camera_update[camID]) {
                    Robot robot = VisionModule::instance()->_camera[camID][0].robot[color][robotId];
                    if (robot.valid){
                        robotSequence[color][robot.id][camID] = robot;
                    }
                }
            }
        }
    }

}
