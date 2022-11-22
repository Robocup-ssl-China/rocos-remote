#ifndef CDEALROBOT_H
#define CDEALROBOT_H

#include "zos/utils/singleton.h"
#include "messageformat.h"
#include "staticparams.h"

class CDealRobot
{
public:
    CDealRobot();

    void run();

    Robot finalSequence[PARAM::TEAMS][PARAM::ROBOTNUM];

private:

    void init();
    void mergeRobot();

    Robot robotSequence[PARAM::TEAMS][PARAM::ROBOTNUM][PARAM::CAMERA];


};

typedef Singleton<CDealRobot> DealRobot;

#endif // CDEALROBOT_H
