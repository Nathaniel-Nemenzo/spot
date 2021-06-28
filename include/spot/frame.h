/*
    frame.h: robot frame helper methods and values 
*/

#ifndef FRAME_H
#define FRAME_H

#include "bosdyn/api/robot_command_service.grpc.pb.h"
#include "bosdyn/api/geometry.grpc.pb.h"
#include "bosdyn/api/lease_service.grpc.pb.h"

#include <string>
#include <map>
#include <Eigen/Geometry>

using bosdyn::api::Vec2;
using bosdyn::api::SE2Pose;
using bosdyn::api::Vec3;
using bosdyn::api::SE3Pose;
using bosdyn::api::FrameTreeSnapshot;
using bosdyn::api::FrameTreeSnapshot_ParentEdge;

const std::string VISION_FRAME_NAME = "vision";
const std::string BODY_FRAME_NAME = "body";
const std::string GRAV_ALIGNED_BODY_FRAME_NAME = "gpe";
const std::string ODOM_FRAME_NAME = "odom";
const std::string GROUND_PLANE_FRAME_NAME = "gpe";
const std::string HAND_FRAME_NAME = "hand";
const std::string UNKNOWN_FRAME_NAME = "unknown";
const std::string RAYCAST_FRAME_NAME = "walkto_raycast_intersection";

class Frame{
public:
    // Frame();
    FrameTreeSnapshot getFrameTreeSnapshot(double, double, double, double, double, double);
    SE3Pose getSE3Pose(double, double, double, double, double, double);
};

#endif