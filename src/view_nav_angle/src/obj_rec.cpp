//#include "plan_execution/ExecutePlanAction.h"


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

std::vector<darknet_ros_msgs::BoundingBox> history;
std::vector<darknet_ros_msgs::BoundingBox> lastcb;
std::vector<ros::Time> time_stamps;
std::string gridFrameId;
ros::Publisher obj_pub;
tf::StampedTransform bottf;
geometry_msgs::PoseStamped current_pose;
tf::TransformListener *tf_l;


//compare bounding boxes, return largest difference between corresponding corners
int compareBoxes(darknet_ros_msgs::BoundingBox a, darknet_ros_msgs::BoundingBox b) {
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


//callback for darknet
void callback(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg){

    std::vector<darknet_ros_msgs::BoundingBox> obj_list = msg->bounding_boxes;

    //go through objects, check if they should be added
    for (int i = 0; i < obj_list.size(); i++) {
      int add = 0;
      for (int j = 0; j < lastcb.size(); j++) {
        if (lastcb[j].Class.compare(obj_list[i].Class) == 0) {
          int diff = compareBoxes(lastcb[j], obj_list[i]);
          if (diff < 500) {
            add = 1;
            //updating to latest location?
            //time_stamps[j] = msg->header.stamp;
          }
        }
      }
      //add object, publish position of segbot
      if (add == 0) {
        history.push_back(obj_list[i]);
        time_stamps.push_back(msg->image_header.stamp);
        geometry_msgs::PoseStamped ret;

        geometry_msgs::PoseStamped out_pose;
        geometry_msgs::PoseStamped bot_pose;
	  	bot_pose.header.seq = 0;
	  	bot_pose.header.stamp = ros::Time(0);
	  	bot_pose.header.frame_id = "base_link";
	  	bot_pose.pose.position.x = 0;
	  	bot_pose.pose.position.y = 0;
	  	bot_pose.pose.orientation.w = 1;

        try{
	    	ros::Time now = msg->image_header.stamp;
	    	tf_l->waitForTransform(gridFrameId, "base_link", now, ros::Duration(3.0));
	      	tf_l->transformPose(gridFrameId, bot_pose, out_pose);
    	} catch (tf::TransformException ex){
	      ROS_ERROR("%s",ex.what());
	    }

        ret = out_pose;
        ret.header.frame_id = obj_list[i].Class;
        obj_pub.publish(ret);
        //position when object was found
        ROS_INFO("%f %f", out_pose.pose.position.x, out_pose.pose.position.y);
      }
    }

    //clear and repopulate list of object from previous frame
    lastcb.clear();
    for (int p = 0; p < obj_list.size(); p++) 
      lastcb.push_back(obj_list[p]);
}




//obtains frame id of occupancy grid for getting segbot location
void getMapId(const nav_msgs::OccupancyGrid::ConstPtr &grid){
  //ROS_INFO("Grid ID");
  gridFrameId = grid->header.frame_id;
}


int main(int argc, char**argv) {
  ROS_INFO("NODE LAUNCHED");
  ros::init(argc, argv, "obj_location");
  ros::NodeHandle objNode;
  ros::Subscriber darknetSub = objNode.subscribe("/darknet_ros/bounding_boxes", 1, &callback);
  ros::Subscriber mapSub = objNode.subscribe("/level_mux/map", 1, getMapId);
  //ros::Subscriver depthSub = objNode.subscribe("/camera/depth/points", 1, c);
  obj_pub = objNode.advertise<geometry_msgs::PoseStamped>("/obj_rec/position/", 5, true);
  tf::TransformListener tfL(objNode);
  tf_l = &tfL;



  while (ros::ok()) {
    geometry_msgs::PoseStamped bot_pose;
  	bot_pose.header.seq = 0;
  	bot_pose.header.stamp = ros::Time(0);
  	bot_pose.header.frame_id = "base_link";
  	bot_pose.pose.position.x = 0;
  	bot_pose.pose.position.y = 0;
  	bot_pose.pose.orientation.w = 1;
    
    //move to callback
    // try{
    // 	ros::Time now = ros::Time::now();
    // 	tfL.waitForTransform(gridFrameId, "base_link", now, ros::Duration(3.0));
    //   	tfL.transformPose(gridFrameId, bot_pose, current_pose);
    // }
    // catch (tf::TransformException ex){
    //   ROS_ERROR("%s",ex.what());
    // }

    ros::spinOnce();
  }



  return 0;
}
