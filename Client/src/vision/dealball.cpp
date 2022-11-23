#include <eigen3/Eigen/Dense>
#include "dealball.h"
#include "visionmodule.h"
#include "kalmanfilter.h"

CDealBall::CDealBall()
{
    lastFrame = 0;
    currentFrame = 0;
    lastBall.fill(0, 0);
    ballFilter = new KalmanFilter(4);
}

CDealBall::~CDealBall()
{
    delete ballFilter;
}

void CDealBall::init() {
    _cycle = VisionModule::instance()->_maintain.cycle();
    // clear
    for(int i = 0; i < PARAM::BALLNUM; i++){
        for(int j = 0; j < PARAM::CAMERA; j++)
            ballSequence[i][j].fill(-32767, -32767);
    }
    int ballNum = 0;
    for (int i = 0; i < PARAM::CAMERA; i++) {
        if (VisionModule::instance()->_camera_update[i]) {
            ballNum += VisionModule::instance()->_camera[i][0].ballNum;
            for (int j = 0; j < PARAM::BALLNUM; j++) {
                if (j < VisionModule::instance()->_camera[i][0].ballNum){
                    Ball current = VisionModule::instance()->_camera[i][0].ball[j];
                    ballSequence[j][i].fill(current.pos.x(), current.pos.y(), current.height);
                }
                else {
                    ballSequence[j][i].fill(-32767, 32767);
                }
            }
        }
    }
    if (ballNum == 0) {
        validBall = false;
    }
    else {
        validBall = true;
    }
}

void CDealBall::mergeBall() {
    actualBallNum = 0;
    if (!validBall) {
        finalSequence[0] = VisionModule::instance()->_maintain[-1].ball[0];
    }
    else {
        for (int i = 0; i < PARAM::BALLNUM; i++) {
            for (int j = 0; j < PARAM::CAMERA; j++) {
                if (ballSequence[i][j].pos.x() < -30000 || ballSequence[i][j].pos.y() < -30000) {
                    continue;
                }
                int index = -1;
                CGeoPoint BallPos = ballSequence[i][j].pos;
                auto _t_b = ballSequence[i][j];
                double weight_total = VisionModule::instance()->calculateWeight(j, BallPos);
                for (int k = j; k < PARAM::CAMERA; k++) {
                    if (findPair(index, k, BallPos)) {
                        double weight = VisionModule::instance()->calculateWeight(k, ballSequence[index][k].pos);
                        BallPos.setX((BallPos.x() * weight_total + ballSequence[index][k].pos.x() * weight)/(weight_total + weight));
                        BallPos.setY((BallPos.y() * weight_total + ballSequence[index][k].pos.y() * weight)/(weight_total + weight));
                        _t_b.height = ((_t_b.height * weight_total + ballSequence[index][k].height * weight)/(weight_total + weight));
                        weight_total += weight;
                        ballSequence[index][k].fill(-32767, 32767);
                    }
                }
                finalSequence[actualBallNum++].fill(BallPos.x(), BallPos.y(),_t_b.height);
                if (actualBallNum >= PARAM::BALLNUM) break;
            }
        }
    }
}

bool CDealBall::findPair(int &index, int camID, CGeoPoint ballPos) {
    for(int i = 0; i < PARAM::BALLNUM; i++) {
        if(ballSequence[i][camID].pos.dist(ballPos) <= PARAM::BALLMERGEDISTANCE) {
            index = i;
            return true;
        }
    }
    return false;
}

void CDealBall::choseBall() {
    double dis = 99999;
    int id = -1;
    for (int i = 0; i < actualBallNum; i++) {
        if (finalSequence[i].pos.dist(lastBall.pos) < dis) {
            dis = finalSequence[i].pos.dist(lastBall.pos);
            id = i;
        }
    }

    // possibility
    if (lastBall.pos.dist(finalSequence[id].pos) < 2000 * (_cycle - lastFrame)) {
        lastBall = finalSequence[id];
        lastFrame = _cycle;
        lastPossible = 1;
        currentPossible = 0;
    }
    else if (currentBall.pos.dist(finalSequence[id].pos) < 2000 * (_cycle - currentFrame)) {
        currentBall = finalSequence[id];
        currentFrame = _cycle;
        currentPossible += 0.1;
        lastPossible -= 0.05;
    }
    else {
        currentBall = finalSequence[id];
        currentFrame = _cycle;
        currentPossible = 0.1;
        lastPossible -= 0.05;
    }

    if (lastPossible >= currentPossible) {
        result = lastBall;
    }
    else {
        result = currentBall;
        lastBall = currentBall;
        lastFrame = currentFrame;
        lastPossible = currentPossible;
        currentPossible = 0;
    }
    result.valid = validBall;
}

void CDealBall::run() {
    init();
    if (validBall) {
        mergeBall();
//        qDebug() << "fuck1";
        choseBall();

        Eigen::Vector2d observation({result.pos.x(), result.pos.y()});
        Eigen::Vector4d filterState;
        filterState = ballFilter->update(observation);
        result.vel = CVector(filterState[2], filterState[3]);
//        qDebug() << "fuck2";
//        qDebug() << filterState[0] << filterState[1] << filterState[2] << filterState[3];
    }
    else {

    }

}

void CDealBall::updateFilter() {

}
