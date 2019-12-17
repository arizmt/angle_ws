//#include "plan_execution/ExecutePlanAction.h"

#include <actionlib/client/simple_action_client.h>
#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <bwi_obj_rec/robot_motion.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <darknet_ros_msgs/BoundingBox.h>
#include <darknet_ros_msgs/BoundingBoxes.h>
#include <tf/tf.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/transform_datatypes.h>
#include <math.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf/transform_broadcaster.h>

//	typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;

std::vector<sensor_msgs::PointCloud2> pc_buffer(200);
sensor_msgs::PointCloud2 npc;
int buf_index;
using namespace std;
RobotMotion *rm;
string gridFrameId;
vector<pair<geometry_msgs::PoseStamped, vector<pair<darknet_ros_msgs::BoundingBox, string>>>> obj_list;
tf::TransformListener *tf_l;
vector<darknet_ros_msgs::BoundingBox> lastcb;
ros::Time lasttime;
bool drive;
geometry_msgs::PoseStamped targ;
string targclass;

// bool canGo(geometry_msgs::PoseStamped start, geometry_msgs::PoseStamped end, nav_msgs::OccupancyGrid map) {
// 	uint32_t r = map.info.height;
// 	uint32_t c = map.info.width;

// 	double x1 = start.pose.position.x;
// 	double y1 = start.pose.position.y;
// 	double x2 = end.pose.position.x;
// 	double y2 = end.pose.position.y;

// 	//swap?
// 	if (x2 < x1) {
// 		double tmp = x1;
// 		x1 = x2;
// 		x2 = tmp;

// 		tmp = y1;
// 		y1 = y2;
// 		y2 = tmp;
// 	}

// 	double slope = (y2-y1)/(x2-x1);
// 	double intercept = y1 - (x1*slope);

// 	//go through line
// 	double currx = floor(x1);
// 	//double curry = y1;
// 	//figure out if going to +x, +y, or -y (for negative slope)
// 	while() {
// 		//double curry = y1-(y2-y1)(-(currx-x1)/(x2-x1));
// 		double curry = slope*(currx)+intercept;
// 		double curryfl = floor(curry);
// 		double nexty = slope*(currx+1)+intercept;
// 		double nextyfl = floor(nexty);

// 		//moved up/down
// 		if (nextyfl != nextyfl) {

// 		//moved straight
// 		} else {

// 		}
// 	}
	
// }

sensor_msgs::PointCloud2 findPoint(ros::Time time) {
  sensor_msgs::PointCloud2 ret = pc_buffer[0];
  ros::Duration diff = ros::Duration(100);
  for (int i = 0; i < pc_buffer.size(); i++) {
    if (abs((pc_buffer[i].header.stamp - time).toSec()) < diff.toSec()) {
      ret = pc_buffer[i];
      diff = ros::Duration(abs((pc_buffer[i].header.stamp - time).toSec()));
      //ROS_INFO("pc in buffer at %d", i);
    }
  }
  return ret;
}

void pixelTo3DPoint(const sensor_msgs::PointCloud2 pCloud, const int t, const int y, geometry_msgs::TransformStamped* p)
{
	//ROS_INFO("get pixel");
  // get width and height of 2D point cloud data
  int width = pCloud.width;
  int height = pCloud.height;
  // Convert from u (column / width), v (row/height) to position in array
  // where X,Y,Z data starts
  int arrayPosition = y*pCloud.row_step + t*pCloud.point_step;
  ROS_INFO("width %d, height %d", t, y);
  // compute position in array where x,y,z data start
  int arrayPosX = arrayPosition + pCloud.fields[0].offset; // X has an offset of 0
  int arrayPosY = arrayPosition + pCloud.fields[1].offset; // Y has an offset of 4
  int arrayPosZ = arrayPosition + pCloud.fields[2].offset; // Z has an offset of 8
  // int arrayPosX = arrayPosition; // X has an offset of 0
  // int arrayPosY = arrayPosition + 4; // Y has an offset of 4
  // int arrayPosZ = arrayPosition + 8; // Z has an offset of 8
  ROS_INFO("1");
  float X = 0.0;
  float Y = 0.0;
  float Z = 0.0;
  uint test = pCloud.data[0];
  //ROS_INFO("2");
  memcpy(&Y, &pCloud.data[arrayPosX], sizeof(float));
  memcpy(&Z, &pCloud.data[arrayPosY], sizeof(float));
  memcpy(&X, &pCloud.data[arrayPosZ], sizeof(float));
  //ROS_INFO("3");

  // put data into the point p
  // x, y, and z are swapped and y and z are negated, rviz orientation
  p->transform.translation.x = X;
  p->transform.translation.y = -Y;
  p->transform.translation.z = -Z;

  p->transform.rotation.x = 0;
  p->transform.rotation.y = 0;
  p->transform.rotation.z = 0;
  p->transform.rotation.w = 1;

  p->header = pCloud.header;
  p->child_frame_id = "obj";
  ROS_INFO("got pixel, x: %f, y: %f, z: %f", X, -Y, -Z);
  ROS_INFO("pcl frame: %s", pCloud.header.frame_id.c_str());
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(-Z, 0, X) );
  transform.setRotation( tf::Quaternion(0, 0, 0, 1) );
  static tf::TransformBroadcaster br;
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "nav_kinect_rgb_optical_frame", "targobj"));
  ROS_INFO("sent transform");
 
 
 /*
 for (int u = 0; u  <pCloud.width; u++) {
			for (int v = 0; v  <pCloud.height; v++) {
			int sarrayPosition = v*pCloud.row_step + u*pCloud.point_step;
			//ROS_INFO("width %d, height %d", u, v);
			// compute position in array where x,y,z data start
			int sarrayPosX = sarrayPosition + pCloud.fields[0].offset; // X has an offset of 0
			int sarrayPosY = sarrayPosition + pCloud.fields[1].offset; // Y has an offset of 4
			int sarrayPosZ = sarrayPosition + pCloud.fields[2].offset; // Z has an offset of 8
			  float sX = 0.0;
				float sY = 0.0;
				float sZ = 0.0;
				memcpy(&sY, &pCloud.data[sarrayPosX], sizeof(float));
				memcpy(&sZ, &pCloud.data[sarrayPosY], sizeof(float));
				memcpy(&sX, &pCloud.data[sarrayPosZ], sizeof(float));
			ROS_INFO("got pixel, x: %f, y: %f, z: %f", sX, sY, sZ);
			}
			}
	*/

  /*
  for (int i = 0; i < (pCloud.row_step*pCloud.height); i++) {
  	ROS_INFO("pointcloud data: %d", pCloud.data[i]);
  }
  */
}

void getMiddle(geometry_msgs::TransformStamped* transform, darknet_ros_msgs::BoundingBox box, sensor_msgs::PointCloud2 pcloud) {
  pixelTo3DPoint(pcloud, (box.xmin + box.xmax)/2, (box.ymin + box.ymax)/2, transform);
  transform->header.frame_id = "camera_link";
  transform->child_frame_id = box.Class;
}

void createNumList(vector<darknet_ros_msgs::BoundingBox> a, vector<pair<darknet_ros_msgs::BoundingBox, string>> b) {
	for (int i = 0; i < a.size(); i++) {
		pair<darknet_ros_msgs::BoundingBox, string> temp(a[i], a[i].Class + to_string(i));
		b.push_back(temp);
	}
}

void createNumList(vector<pair<bool, darknet_ros_msgs::BoundingBox>> a,  vector<pair<bool, pair<darknet_ros_msgs::BoundingBox, string>>> *b) {
	for (int i = 0; i < a.size(); i++) {
		pair<darknet_ros_msgs::BoundingBox, string> temp(a[i].second, a[i].second.Class + to_string(i));
		pair<bool, pair<darknet_ros_msgs::BoundingBox, string>> add(a[i].first, temp);
		b->push_back(add);
	}
}

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

void matchBox(vector<darknet_ros_msgs::BoundingBox> a, vector<pair<darknet_ros_msgs::BoundingBox, string>> b, vector<pair<bool, darknet_ros_msgs::BoundingBox>> *result) {
	vector<pair<bool, darknet_ros_msgs::BoundingBox>> check;
	for (int p = 0; p < b.size(); p++) {
		pair<bool, darknet_ros_msgs::BoundingBox> temp(false, b[p].first);
		check.push_back(temp);
	}
	for (int i = 0; i < a.size(); i++) {
		bool found = false;
		for (int j = 0; j < check.size() && !found; j++) {
			if (!check[j].first && check[j].second.Class.compare(a[i].Class) == 0) {
				found = true;
				check[j].first = true;
			}
		}
		if (!found) {
			pair<bool, darknet_ros_msgs::BoundingBox> temp(true, a[i]);
			result->push_back(temp);
		}
	}
	for (int k = 0; k < check.size(); k++) {
		if (check[k].first == false) {
			pair<bool, darknet_ros_msgs::BoundingBox> temp(false, check[k].second);
			
			result->push_back(temp);
		}
	}
}

geometry_msgs::PoseStamped* circle(geometry_msgs::PoseStamped obj, double dist) {
	static geometry_msgs::PoseStamped circ[8];
	obj.pose.orientation.w = 1;
	circ[0] = geometry_msgs::PoseStamped(obj);
	circ[0].pose.position.x += dist;
	circ[2] = geometry_msgs::PoseStamped(obj);
	circ[2].pose.position.y += dist;
	circ[4] = geometry_msgs::PoseStamped(obj);
	circ[4].pose.position.x -= dist;
	circ[6] = geometry_msgs::PoseStamped(obj);
	circ[6].pose.position.y -= dist;

	circ[1] = geometry_msgs::PoseStamped(obj);
	circ[1].pose.position.x += dist/sqrt(2);
	circ[1].pose.position.y += dist/sqrt(2);
	circ[3] = geometry_msgs::PoseStamped(obj);
	circ[3].pose.position.x -= dist/sqrt(2);
	circ[3].pose.position.y += dist/sqrt(2);
	circ[5] = geometry_msgs::PoseStamped(obj);
	circ[5].pose.position.x -= dist/sqrt(2);
	circ[5].pose.position.y -= dist/sqrt(2);
	circ[7] = geometry_msgs::PoseStamped(obj);
	circ[7].pose.position.x += dist/sqrt(2);
	circ[7].pose.position.y -= dist/sqrt(2);

	return circ;
	
}

void moveCircle(geometry_msgs::PoseStamped obj, string clas) {
	geometry_msgs::PoseStamped* inner = circle(obj, .2);
	geometry_msgs::PoseStamped* outer = circle(obj, .5);
	ROS_INFO("obj x: %f, obj y: %f", obj.pose.position.x, obj.pose.position.y);
	ROS_INFO("got circle points");
	obj.pose.orientation.w = 1;
	rm->move_to_pose(obj);
	while (rm->action_status());
	/*
	for (int i = 0; i < 8; i++) {
		ROS_INFO("targ x: %f, targ y: %f", outer[i].pose.position.x, outer[i].pose.position.y);
		rm->move_to_pose(outer[i]);
		while (rm->action_status());
		rm->move_to_pose(inner[i]);
		while (rm->action_status()) { ros::spinOnce(); }
		rm->move_to_pose(outer[i]);
		while (rm->action_status());
		
	}
	
	*/
	drive = false;
}

void darkcb(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg){
	if (!drive) {
		// start driving around object
		
		vector<darknet_ros_msgs::BoundingBox> msg_list = msg->bounding_boxes;
		ROS_INFO("starting circle");
		sensor_msgs::PointCloud2 currpc = findPoint(msg->header.stamp);
		ROS_INFO("got cloud");
		darknet_ros_msgs::BoundingBox box = msg_list[0];
    geometry_msgs::TransformStamped transform;
		getMiddle(&transform, box, currpc);
		//transform stores 3d pose estimate
		geometry_msgs::PoseStamped center;
		center.pose.position.x = transform.transform.translation.x;
		center.pose.position.y = transform.transform.translation.y;
		center.pose.position.z = transform.transform.translation.z;
		targ = center;
		targclass = box.Class;
		ROS_INFO("target object: %s", msg->bounding_boxes[0].Class.c_str());
		drive = true;

	} else {
		ROS_INFO("YAY");
	}

}

void getMapId(const nav_msgs::OccupancyGrid::ConstPtr &grid){
  ROS_INFO("Creating RobotMotion");
  tf::TransformListener *listener = new tf::TransformListener();
  gridFrameId = grid->header.frame_id;
  rm = new RobotMotion(gridFrameId, *listener);
}

void getPose(pair<float, float> coord, geometry_msgs::PoseStamped pose) {
	pose.pose.position.x = coord.first;
	pose.pose.position.y = coord.second;
	pose.pose.position.z = 0;
	pose.pose.orientation.x = 0;
	pose.pose.orientation.y = 0;
	pose.pose.orientation.z = 0;
	pose.pose.orientation.w = 1;
}



//callback to store point clouds
void getPoint(const sensor_msgs::PointCloud2ConstPtr& msg) {
	 //ROS_INFO("get point");
	 //ROS_INFO("wid: %d, hei: %d", msg->width, msg->height);
    if (buf_index >= 200 || buf_index < 0) {
      buf_index = 0;
    }
    pc_buffer[buf_index] = *msg;
    buf_index++;
    //ROS_INFO("got point");
    npc = *msg;
    
    	sensor_msgs::PointCloud2 pCloud = *msg;
      
}




int main(int argc, char**argv) {
	ros::init(argc, argv, "move_to_location");
  ros::NodeHandle moveNode;
  ROS_INFO("SDF");
	
	ROS_INFO("FDS");

  ros::Subscriber mapSub = moveNode.subscribe("/level_mux/map", 1, getMapId);
  ros::Subscriber darknetSub = moveNode.subscribe("/darknet_ros/bounding_boxes", 1, darkcb);
  ros::Subscriber pointSub = moveNode.subscribe("/nav_kinect/depth_registered/points", 1, getPoint);
  tf::TransformListener tfL(moveNode);
  tf_l = &tfL;
  drive = false;

  vector<geometry_msgs::PoseStamped> location_list;
  //pair<float, float> soccer_lab_door_north(-47.7036, -11.2160);
  //visit_door_list
  pair<float, float> d3_414a1(-19.8, -4.9);
  pair<float, float> d3_414a2(-20.15, -11.9);
  pair<float, float> d3_414b1(-38.45, -4.4);
  pair<float, float> d3_414b2(-38.9, -11.55);
  pair<float, float> d3_418(-22.3, -12.2);
  geometry_msgs::PoseStamped kitchen;
  kitchen.pose.position.x = -32.035;
  kitchen.pose.position.y = -3.1599;
  kitchen.pose.position.z = 0;
  kitchen.pose.orientation.x = 0;
  kitchen.pose.orientation.y = 0;
  kitchen.pose.orientation.z = 0.1;
  kitchen.pose.orientation.w = 0.994987;
  geometry_msgs::PoseStamped lounge;
  lounge.pose.position.x = -31.205;
  lounge.pose.position.y = -4.0182;
  lounge.pose.position.z = 0;
  lounge.pose.orientation.x = 0;
  lounge.pose.orientation.y = 0;
  lounge.pose.orientation.z = 0.694381;
  lounge.pose.orientation.w = 0.719607;
  geometry_msgs::PoseStamped d414a;
  d414a.pose.position.x = d3_414a1.first;
	d414a.pose.position.y = d3_414a1.second;
	d414a.pose.position.z = 0;
	d414a.pose.orientation.w = 1;
	location_list.push_back(lounge);
  //location_list.push_back(d414a);
  //location_list.push_back(kitchen);
  //location_list.push_back(d3_414a2);
  geometry_msgs::PoseStamped d414b;
  d414b.pose.position.x = d3_414b1.first;
	d414b.pose.position.y = d3_414b1.second;
	d414b.pose.position.z = 0;
	d414b.pose.orientation.w = 1;
  //location_list.push_back(d414b);
  //location_list.push_back(d3_414b2);
  //location_list.push_back(d3_418);
  int location_index = 0;
  int obj_index = 0;
  //vector<geometry_msgs::PoseStamped> demonav = circ(lounge, 2.0);

  ros::Duration(2).sleep();
  ros::spinOnce();
  ROS_INFO("starting initial loop");
  ROS_INFO("Moving to location 0");
  rm->move_to_pose(location_list[location_index]);
  //location_index++;
  while (ros::ok())
  {
	  if (0 != rm && !rm->action_status())
	  {
		  //initial loop
		  if (!drive) {
			  ROS_INFO("Moving to location %d", location_index);
			  rm->move_to_pose(location_list[location_index]);
			  //location_index++;
				if (location_index >= location_list.size()) {
					location_index = 0;
				}
		  } else {
		  	ROS_INFO("moving circle");
		  	tf::StampedTransform targtf;
		  	bool good = true;
		  	try{
				  tfL.lookupTransform("level_mux_map", "targobj",  
				                           ros::Time(0), targtf);
				}
				catch (tf::TransformException ex){
				  ROS_ERROR("%s",ex.what());
				  good = false;
				  drive = false;
				  ros::Duration(1.0).sleep();
				}
				if (good) {
					geometry_msgs::PoseStamped targp;
					targp.pose.position.x = targtf.getOrigin().getX();
					targp.pose.position.y = targtf.getOrigin().getY();
					targp.pose.position.z = targtf.getOrigin().getZ();
			  	moveCircle(targp, targclass);
			  }
		  }
		

		  //ros::Duration(3).sleep();
	  }
	  else
	  {
		  ros::Time end = ros::Time::now() + ros::Duration(5);
		  ros::Time start = ros::Time::now();
		  while (ros::Time::now().toSec() < end.toSec())
		  {
			  ros::spinOnce();
		  }
	  }

	  ros::spinOnce();
  }

  return 0;
}

