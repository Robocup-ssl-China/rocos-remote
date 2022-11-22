#ifndef CDEALBALL_H
#define CDEALBALL_H

#include "zos/utils/singleton.h"
#include "messageformat.h"

class KalmanFilter;

class CDealBall
{
public:
    CDealBall();
    ~CDealBall();
    void run();
    void init();

    Ball lastBall, currentBall, result;

private:
    void mergeBall();
    void choseBall();
    void updateFilter();
    bool findPair(int& index, int camID, CGeoPoint ballPos);

    Ball ballSequence[PARAM::BALLNUM][PARAM::CAMERA];
    Ball finalSequence[PARAM::BALLNUM];

    int actualBallNum;
    long long _cycle, lastFrame, currentFrame;
    double lastPossible, currentPossible;
    bool validBall;

    KalmanFilter* ballFilter;

};

typedef Singleton<CDealBall> DealBall;

#endif // CDEALBALL_H
