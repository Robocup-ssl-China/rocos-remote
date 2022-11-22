#include <QDebug>
#include <iostream>
#include "kalmanfilter.h"
#include "staticparams.h"

KalmanFilter::KalmanFilter(int dim) :
    initialized(false)
{
    K.resize(dim, dim / 2);
    A.resize(dim, dim);
    P.resize(dim, dim);
    Q.resize(dim, dim);
    H.resize(dim / 2, dim);
    R.resize(dim / 2, dim / 2);
    _x.resize(dim);

    R.setIdentity();
    R = R * 20;
//    qDebug() << H.rows();
    for (int i = 0; i < H.rows(); i++) {
        H(i, i) = 1;
    }
    A.setIdentity();
    for (int i = 0; i < dim / 2; i++) {
        A(i, i + dim / 2) = 1 / PARAM::Vision::FRAME_RATE;
    }
    Q.setIdentity();
    Q = Q * 2;
}

void KalmanFilter::init(Eigen::VectorXd state) {
    for (int i=0; i<state.size(); i++) {
        _x[i] = state[i];
    }

    P.setIdentity();
    P = P * 5;

    initialized = true;
}

const Eigen::VectorXd& KalmanFilter::update(Eigen::VectorXd measurement) {
    if (!initialized) {
        init(measurement);
    }
    Eigen::VectorXd z;
    z.resizeLike(measurement);
    Eigen::VectorXd x_;
    x_.resizeLike(_x);
    Eigen::MatrixXd P_;
    P_.resizeLike(A);

    z = measurement;
    x_ = A * _x;
    P_ = A * P * A.transpose() + Q;
    K = P_ * H.transpose() * (H * P_ * H.transpose() + R).inverse();
    _x = x_ + K * (z - H * x_);
    P = P_ - K * H * P_;
    return _x;
}

