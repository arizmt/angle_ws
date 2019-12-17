#ifndef BWI_SCAVENGER_ROBOT_MOTION_H
#define BWI_SCAVENGER_ROBOT_MOTION_H

#include <ros/ros.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <darknet_ros_msgs/BoundingBox.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/transform_datatypes.h>



class ovj_rec {
protected:
  std::vector<darknet_ros_msgs::BoundingBox> history;
  std::vector<darknet_ros_msgs::BoundingBox> lastcb;
  std::vector<ros::Time> time_stamps;
  std::string gridFrameId;
  ros::Publisher obj_pub;
  tf::StampedTransform bottf;

public:
  int compareBoxes(darknet_ros_msgs::BoundingBox a, darknet_ros_msgs::BoundingBox b);

  void callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg)

  void getMapId(const nav_msgs::OccupancyGrid::ConstPtr &grid)
};

#endif
