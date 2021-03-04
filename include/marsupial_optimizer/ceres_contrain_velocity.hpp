#ifndef CERES_CONTRAIN_VELOCITY_HPP
#define CERES_CONTRAIN_VELOCITY_HPP


#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen/Core"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solve;
using ceres::Solver;



class VelocityFunctor {

public:
  VelocityFunctor(double weight_factor, double init_vel_ugv, double init_vel_uav): wf_(weight_factor), iv_ugv(init_vel_ugv), iv_uav(init_vel_uav) {}

  template <typename T>
  bool operator()(const T* const statePos1, const T* const statePos2, const T* const stateT, T* residual) const {

	T d_ugv_ = sqrt(pow(statePos2[1]-statePos1[1],2) + pow(statePos2[2]-statePos1[2],2)) ;
  T dt_ugv_ = stateT[1];
	T v_ugv_ = d_ugv_ / dt_ugv_;

  T d_uav_ = sqrt(pow(statePos2[4]-statePos1[4],2) + pow(statePos2[5]-statePos1[5],2) + pow(statePos2[6]-statePos1[6],2)) ;
  T dt_uav_ = stateT[2];
	T v_uav_ = d_uav_ / dt_uav_;

	residual[0] =  wf_ * (v_ugv_ - iv_ugv);
	residual[1] =  wf_ * (v_uav_ - iv_uav);

  return true;
  }

 double wf_, iv_ugv, iv_uav;

 private:
};


#endif