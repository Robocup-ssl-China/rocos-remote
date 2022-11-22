#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <eigen3/Eigen/Dense>
#include <geometry.h>

class KalmanFilter
{
    // position and velocity
public:
    KalmanFilter(int dim);
    void init(Eigen::VectorXd);
    const Eigen::VectorXd& update(Eigen::VectorXd);
    const Eigen::VectorXd& getState() {return _x;}

private:
    Eigen::MatrixXd K, P, A, Q, H, R;
    Eigen::VectorXd _x;

    bool initialized;
};

#endif // KALMANFILTER_H
