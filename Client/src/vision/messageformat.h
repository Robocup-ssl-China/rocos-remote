#ifndef __MESSAGEFORMAT_H__
#define __MESSAGEFORMAT_H__

#include "geometry.h"
#include "staticparams.h"

struct Ball {
    CGeoPoint pos;
    CVector vel;
    bool valid = false;
    Ball() : pos() {}
    void fill(double x, double y) {
        this->pos.setX(x);
        this->pos.setY(y);
    }
    void fill(double x, double y, CVector vel) {
        this->pos.setX(x);
        this->pos.setY(y);
        this->vel = vel;
        this->valid = true;
    }
};

struct Robot {
    unsigned short id;
    CGeoPoint pos;
    double angle;
    CVector vel;
    double rotvel;
    bool valid = false;
    Robot() : id(-1), pos(-99999, -99999) {}
    Robot(double x, double y, double angle, int id = -1) {
        pos.setX(x);
        pos.setY(y);
        this->angle = angle;
        this->id = id;
    }
    void fill(unsigned short id, double x, double y, double angle, CVector vel = CVector(0, 0)) {
        this->id = id;
        this->pos.setX(x);
        this->pos.setY(y);
        this->angle = angle;
        this->vel = vel;
        valid = true;
    }
};

struct CameraEdge {
    CGeoPoint center, leftUp, rightDown;
    bool valid = false;
    CameraEdge() {}
    void update(CGeoPoint p, int type=0) {
        double extraDist = type == 0 ? PARAM::Vehicle::V2::PLAYER_SIZE : PARAM::Field::BALL_SIZE;
        if (! valid) {
            center = p;
            leftUp = p + CVector(-extraDist, extraDist);
            rightDown = p + CVector(extraDist, -extraDist);
        }
        else {
            if (p.x() < leftUp.x()) {
                leftUp.setX(p.x()-extraDist);
            }
            if (p.y() > leftUp.y()) {
                leftUp.setY(p.y()+extraDist);
            }
            if (p.x() > rightDown.x()) {
                rightDown.setX(p.x()+extraDist);
            }
            if (p.y() < rightDown.y()) {
                rightDown.setY(p.y()-extraDist);
            }
        }
    }
};

class VisionMessage {
public:
    unsigned long long frame;
    unsigned short robotNum[2];
    unsigned short ballNum;
    Robot robot[2][PARAM::ROBOTNUM];
    Ball ball[PARAM::BALLNUM];
    VisionMessage() : frame(-1), ballNum(0) {
        init();
    }
    void init() {
        robotNum[PARAM::BLUE] = robotNum[PARAM::YELLOW] = 0;
        ball[0].fill(-32767, -32767);
    }
    bool addBall(double x, double y) {
        if (ballNum >= PARAM::BALLNUM) {
            return false;
        }
        ball[ballNum++].fill(x, y);
        return true;
    }
    bool addBall(const Ball& b) {
        if (ballNum >= PARAM::BALLNUM) {
            return false;
        }
        ball[ballNum++] = b;
        return true;
    }
    bool addRobot(int color, unsigned short id, double x, double y, double angle) {
        if (robotNum[color] >= PARAM::ROBOTNUM) {
            return false;
        }
        robot[color][robotNum[color]++].fill(id, x, y, angle);
        return true;
    }
    void clear() {
        frame = -1;
        ballNum = 0;
        robotNum[PARAM::BLUE] = robotNum[PARAM::YELLOW] = 0;
    }

};

#endif // __MESSAGEFORMAT_H__
