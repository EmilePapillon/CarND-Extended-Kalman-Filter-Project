#include "kalman_filter.h"
#include "tools.h"
#ifndef CMATH_
#include <cmath>
#define CMATH_
#endif

//remove following line before flight
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

// Please note that the Eigen library does not initialize 
// VectorXd or MatrixXd objects with zeros upon creation.

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ =F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd S = H_ * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z, Tools &tools) {
  /**
  TODO:
    * update the state by using Extended Kalman Filter equations
  */
//use the measurement function h to calculate error
  float px= x_[0];
  float py= x_[1];
  float px_dot= x_[2];
  float py_dot= x_[3];

  VectorXd z_pred(3);
  float rho = sqrt(px*px + py*py);
  float theta = atan2(py,px);
  float rho_dot = (px*px_dot + py*py_dot) / rho ;
  if (theta > M_PI) {
    cout << "ERROR: normalization of the angle did not work properly" << endl;
  }
  z_pred << rho, theta, rho_dot;
  VectorXd err = z - z_pred;
  VectorXd y(3);
  y << err[0],tools.Normalize(err[1]),err[2];
//use jacobian to calculate update

  //MatrixXd H_j = H_;
  MatrixXd H_j = tools.CalculateJacobian(x_);
  MatrixXd Ht = H_j.transpose();
  MatrixXd S = H_j * P_ * Ht + R_;
  MatrixXd Si = S.inverse();
  MatrixXd PHt = P_ * Ht;
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_j) * P_;
}
