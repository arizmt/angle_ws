//#include "plan_execution/ExecutePlanAction.h"


#include <ros/ros.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <darknet_ros_msgs/BoundingBox.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/PointField.h>
#include <tf/transform_datatypes.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/TransformStamped.h>
#include <algorithm>

tf::TransformBroadcaster* tfb;
std::vector<std::pair<std::string, int>> obj_count;
std::vector<std::pair<std::string, geometry_msgs::TransformStamped>> obj_list;
darknet_ros_msgs::BoundingBoxes prev;

int maxBoxDiff(darknet_ros_msgs::BoundingBox a, darknet_ros_msgs::BoundingBox b){
  int maxdiff = 0;
  int xmin = abs(a.xmin - b.xmin);
  int ymin = abs(a.ymin - b.ymin);
  int xmax = abs(a.xmax - b.xmax);
  int ymax = abs(a.ymax - b.ymax);
  maxdiff = xmin > maxdiff ? xmin : maxdiff;
  maxdiff = ymin > maxdiff ? ymin : maxdiff;
  maxdiff = xmax > maxdiff ? xmax : maxdiff;
  maxdiff = ymax > maxdiff ? ymax : maxdiff;
  return maxdiff;
}

void compareBoxes(darknet_ros_msgs::BoundingBoxes prev, darknet_ros_msgs::BoundingBoxes curr) {
  for (int i = 0; i < prev.bounding_boxes.size(); i++) {
    bool found = false;
    for (int j = 0; j < curr.bounding_boxes.size() && !found; j++) {
      if (prev.bounding_boxes[i].Class.compare(curr.bounding_boxes[j].Class) == 0) {
        int diff = maxBoxDiff(prev.bounding_boxes[i], curr.bounding_boxes[j]);
        if (diff < 50) {
          found = true;
        }
      }
    }
    if (!found) {
      ROS_INFO("%s missing", prev.bounding_boxes[i].Class.c_str());
    }
  } 
}

void callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg){
    std::vector<darknet_ros_msgs::BoundingBox> obj_list = msg->bounding_boxes;
    //ROS_INFO("get darknet");
    //ROS_INFO("found point");
    //go through objects, check if they should be added
    
     //ROS_INFO("got darknet");
}


int main(int argc, char**argv) {
  ROS_INFO("NODE LAUNCHED");
  ros::init(argc, argv, "obj_track");
  ros::NodeHandle objNode;
  ros::Subscriber darknetSub = objNode.subscribe("/darknet_ros/bounding_boxes", 1, &callback);
  //obj_pub = &objNode.advertise<geometry_msgs::PoseStamped>("/obj_rec/position/", 5, true);
  tf::TransformBroadcaster tf_b;
  tfb = &tf_b;
  while (ros::ok()) {
    ros::spinOnce();
  }



  return 0;
}
