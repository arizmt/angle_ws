//#include "plan_execution/ExecutePlanAction.h"

#include <actionlib/client/simple_action_client.h>

#include <ros/ros.h>
#include <tf/transform_listener.h>

#include <bwi_obj_rec/robot_motion.h>

#include <iostream>

//	typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;

using namespace std;

RobotMotion *rm;
std::string gridFrameId;
std::vector<std::pair<float, float>> objloc_list;

void getMapId(const nav_msgs::OccupancyGrid::ConstPtr &grid){
  ROS_INFO("Creating RobotMotion");
  tf::TransformListener *listener = new tf::TransformListener();
  gridFrameId = grid->header.frame_id;
  rm = new RobotMotion(gridFrameId, *listener);
}

void objCB(const geometry_msgs::PoseStamped::ConstPtr &msg) {
  std::pair<float, float> objloc(msg->pose.position.x, msg->pose.position.y);
  objloc_list.push_back(objloc);
}


int main(int argc, char**argv) {
  ros::init(argc, argv, "move_to_location");
  ros::NodeHandle moveNode;

  ros::Subscriber mapSub = moveNode.subscribe("/level_mux/map", 1, getMapId);
  ros::Subscriber objSub = moveNode.subscribe("/obj_rec/position/", 1, objCB);
  std::vector<std::pair<float, float>> location_list;
  //std::pair<float, float> soccer_lab_door_north(-47.7036, -11.2160);
  //visit_door_list
  std::pair<float, float> d3_414a1(-19.8, -4.9);
  std::pair<float, float> d3_414a2(-20.15, -11.9);
  std::pair<float, float> d3_414b1(-38.45, -4.4);
  std::pair<float, float> d3_414b2(-38.9, -11.55);
  std::pair<float, float> d3_418(-22.3, -12.2);
  location_list.push_back(d3_414a1);
  location_list.push_back(d3_414a2);
  location_list.push_back(d3_414b1);
  location_list.push_back(d3_414b2);
  location_list.push_back(d3_418);
  int location_index = 0;



  while (ros::ok()) {
  if (0 != rm)
  {
     ROS_INFO("Moving to location %d", location_index);
     rm->move_to_location(location_list[location_index]);
     ros::Duration(3).sleep();
  }

  location_index++;
  if (location_index >= location_list.size())
    location_index = 0;


  if (objloc_list.size() > 0) {
    rm->move_to_location(objloc_list[0]);
    ros::Duration(3).sleep();
  } else {

  }

  ros::spinOnce();
  }



  return 0;
}
