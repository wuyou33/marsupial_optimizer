#ifndef CERES_CONTRAIN_OBSTACLES_HPP
#define CERES_CONTRAIN_OBSTACLES_HPP

#include "ceres/ceres.h"
#include "glog/logging.h"
#include "Eigen/Core"

#include <pcl/search/impl/kdtree.hpp>
#include <pcl/kdtree/impl/kdtree_flann.hpp>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/point_types.h>

#include "misc/near_neighbor.hpp"

using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solve;
using ceres::Solver;


class ObstacleDistanceFunctor {

public:
    ObstacleDistanceFunctor(){}
    
    struct ComputeDistanceObstacles 
    {
        ComputeDistanceObstacles (pcl::KdTreeFLANN <pcl::PointXYZ> kdT_From_NN, pcl::PointCloud <pcl::PointXYZ>::Ptr obstacles_Points)
        : kdT_(kdT_From_NN), o_p_(obstacles_Points)
        {
    
        }

        bool operator()(const double *state1, 
                        double *near_) const 
        {
            NearNeighbor nn;

            nn.nearestObstacleStateCeres(kdT_ , state1[4],state1[5], state1[6], o_p_, near_[0], near_[1], near_[2]);
            return true;
        }

        double f_, sb_;
        pcl::KdTreeFLANN <pcl::PointXYZ> kdT_;
        pcl::PointCloud <pcl::PointXYZ>::Ptr o_p_;
    };


    struct ObstaclesFunctor 
    {
        ObstaclesFunctor(double weight_factor, double safty_bound, pcl::KdTreeFLANN <pcl::PointXYZ> kdT_From_NN, pcl::PointCloud <pcl::PointXYZ>::Ptr obstacles_Points)
            : wf_(weight_factor), sb_(safty_bound), kdT_(kdT_From_NN), o_p_(obstacles_Points)
        {
            compute_nearest_distance.reset(new ceres::CostFunctionToFunctor<7,4>(
                                    new ceres::NumericDiffCostFunction<ComputeDistanceObstacles,
                                                                        ceres::CENTRAL, 
                                                                        7,
                                                                        4>( 
                                    new ComputeDistanceObstacles(kdT_, o_p_))));
        }

        template <typename T>
        bool operator()(const T* const pose1, 
                        T* residual) const 
        {
            T d_ugv_;
            T d_uav_;
            T n_[4];
            (*compute_nearest_distance)(pose1, n_);

            // d_ugv_ = ((pose1[1]-n_[0])*(pose1[1]-n_[0]) + (pose1[2]-n_[1])*(pose1[2]-n_[1]) + (pose1[3]-n_[2])*(pose1[3]-n_[2]));
            d_uav_ = ((pose1[4]-n_[0])*(pose1[4]-n_[0]) + (pose1[5]-n_[1])*(pose1[5]-n_[1]) + (pose1[6]-n_[2])*(pose1[6]-n_[2]));

            // residual[0] =  wf_ * exp(sb_*sb_ - 2.0*d_ugv_);
            residual[0] =  wf_ * exp(sb_*sb_ - 2.0*d_uav_);

            return true;
        }

        std::unique_ptr<ceres::CostFunctionToFunctor<7,4> > compute_nearest_distance;
        double wf_, sb_;
        pcl::KdTreeFLANN <pcl::PointXYZ> kdT_;
        pcl::PointCloud <pcl::PointXYZ>::Ptr o_p_;
    };


private:

};

#endif