#ifndef CERES_CONTRAIN_OBSTACLES_HPP
#define CERES_CONTRAIN_OBSTACLES_HPP


#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen/Core"

#include <pcl/search/impl/kdtree.hpp>
#include <pcl/kdtree/impl/kdtree_flann.hpp>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/point_types.h>

#include "marsupial_g2o/near_neighbor.hpp"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solve;
using ceres::Solver;


struct ObstacleDistanceFunctor {
  ObstacleDistanceFunctor(double weight_factor, double safty_bound, pcl::KdTreeFLANN <pcl::PointXYZ> kdT_From_NN, pcl::PointCloud <pcl::PointXYZ>::Ptr obstacles_Points)
        : wf_(weight_factor), sb_(safty_bound), kdT_(kdT_From_NN), o_p_(obstacles_Points)
    {
    }

    bool operator()(const double* state1, double* residual) const 
    {
        double d_[1];
        NearNeighbor nn;
        double near_[4];
        
        nn.nearestObstacleVertexCeres(kdT_ , state1[0],state1[1], state1[2], o_p_, near_[0], near_[1], near_[2]);
        d_[0] = ((state1[0]-near_[0])*(state1[0]-near_[0]) + (state1[1]-near_[1])*(state1[1]-near_[1]) + (state1[2]-near_[2])*(state1[2]-near_[2]));
        residual[0] =  wf_ * exp(sb_ - 4.0*d_[0]);

        return true;
    }

    double wf_, sb_;
    pcl::KdTreeFLANN <pcl::PointXYZ> kdT_;
    pcl::PointCloud <pcl::PointXYZ>::Ptr o_p_;
};


#endif