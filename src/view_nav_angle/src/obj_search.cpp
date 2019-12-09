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

//	typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;

std::vector<sensor_msgs::PointCloud2> pc_buffer(200);
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
    }
  }
  return ret;
}

void pixelTo3DPoint(const sensor_msgs::PointCloud2 pCloud, const int u, const int v, geometry_msgs::TransformStamped* p)
{
	//ROS_INFO("get pixel");
  // get width and height of 2D point cloud data
  int width = pCloud.width;
  int height = pCloud.height;
  // Convert from u (column / width), v (row/height) to position in array
  // where X,Y,Z data starts
  int arrayPosition = v*pCloud.row_step + u*pCloud.point_step;
  //ROS_INFO("0");
  // compute position in array where x,y,z data start
  int arrayPosX = arrayPosition + pCloud.fields[0].offset; // X has an offset of 0
  int arrayPosY = arrayPosition + pCloud.fields[1].offset; // Y has an offset of 4
  int arrayPosZ = arrayPosition + pCloud.fields[2].offset; // Z has an offset of 8
  // int arrayPosX = arrayPosition; // X has an offset of 0
  // int arrayPosY = arrayPosition + 4; // Y has an offset of 4
  // int arrayPosZ = arrayPosition + 8; // Z has an offset of 8
  //ROS_INFO("1");
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
  //ROS_INFO("got pixel");
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
	geometry_msgs::PoseStamped* inner = circle(obj, .5);
	geometry_msgs::PoseStamped* outer = circle(obj, 2);
	for (int i = 0; i < 8; i++) {
		rm->move_to_pose(outer[i]);
		while (rm->action_status());
		rm->move_to_pose(inner[i]);
		while (rm->action_status()) { ros::spinOnce(); }
		rm->move_to_pose(outer[i]);
		while (rm->action_status());
	}
	drive = false;
}

void darkcb(const darknet_ros_msgs::BoundingBoxes::ConstPtr& msg){
	if (!drive) {
		// start driving around object
		drive = true;
		vector<darknet_ros_msgs::BoundingBox> msg_list = msg->bounding_boxes;
		ROS_INFO("starting circle");
		sensor_msgs::PointCloud2 currpc = findPoint(msg->header.stamp);
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


	} else {
		ROS_INFO("YAY");
	}

	// bool made = false;
	// float distance = 10;
	// geometry_msgs::PoseStamped ret;
	// geometry_msgs::PoseStamped out_pose;

	// if (msg_list.size() > 0) {
	//     //go through objects
	//     for (int i = 0; i < msg_list.size(); i++) {
	      
	//         //check if seen
	//         int add = 0;
	//         if (msg->header.stamp.toSec() - lasttime.toSec() < 4) {
	//           for (int j = 0; j < lastcb.size(); j++) {
	//             if (lastcb[j].Class.compare(msg_list[i].Class) == 0) {
	//               int diff = compareBoxes(lastcb[j], msg_list[i]);
	//               if (diff < 500)
	//                 add = 1;
	//             }
	//           }
	//         }
	//       //add object, position of segbot
	//       if (add != 1 && obj_list.size() <= 6) {
	//       	if (!made) {
		        
	// 	        geometry_msgs::PoseStamped bot_pose;
	// 	        bot_pose.header.seq = 0;
	// 	        bot_pose.header.stamp = ros::Time(0);
	// 	        bot_pose.header.frame_id = "base_link";
	// 	        bot_pose.pose.position.x = 0;
	// 	        bot_pose.pose.position.y = 0;
	// 	        bot_pose.pose.orientation.w = 1;

	// 	        try{
	// 	          ros::Time now = msg->image_header.stamp;
	// 	          tf_l->waitForTransform(gridFrameId, "base_link", now, ros::Duration(1.0));
	// 	          tf_l->transformPose(gridFrameId, bot_pose, out_pose);
	// 	        } catch (tf::TransformException ex){
	// 	          ROS_ERROR("%s",ex.what());
	// 	        }

	// 	        out_pose.header.frame_id = msg_list[i].Class;
	// 	    }

	//         if (add == 0) {
	//           if (!made) {
	// 	          for (int p = 0; p < obj_list.size(); p++) {
	// 	            float x = obj_list[p].first.pose.position.x - out_pose.pose.position.x;
	// 	            float y = obj_list[p].first.pose.position.y - out_pose.pose.position.y;
	// 	            float tempdist = sqrt(pow(x, 2) + pow(y, 2));
	// 	            if (tempdist < distance) {
	// 	              distance = tempdist;
	// 	            }
	// 	          }
	// 	      }

	//           if (distance > 3) {
	//           	if (!made) {
	// 	            ret = out_pose;
	// 	            ret.header.frame_id = msg_list[i].Class;
	// 	            pair<darknet_ros_msgs::BoundingBox, string> boxpair(msg_list[i], msg_list[i].Class + "0");
	// 	            vector<pair<darknet_ros_msgs::BoundingBox, string>> list;
	// 	            list.push_back(boxpair);
	// 	            pair<geometry_msgs::PoseStamped, vector<pair<darknet_ros_msgs::BoundingBox, string>>> temp(ret, list);
	//             	obj_list.push_back(temp);
	//             } else {
	//             	pair<darknet_ros_msgs::BoundingBox, string> boxpair(msg_list[i], msg_list[i].Class + to_string(obj_list[obj_list.size()-1].second.size()));
	//             	obj_list[obj_list.size()-1].second.push_back(boxpair);
	//             }
	//             ROS_INFO("added %s", msg_list[i].Class.c_str());
	//             //position when object was found (x and y)
	//             //ROS_INFO("%f %f", out_pose.pose.position.x, out_pose.pose.position.y);
	//             made = true;
	//           } else {
	//           		//ROS_INFO("too close");
	//           }
	//         } else {
	//           ROS_INFO("update location");
	//           obj_list[i].first = out_pose;
	//         }
	//       	//ROS_INFO("add: %d", add);
	//       }
	//     }

	// }
    // //clear and repopulate list of object from previous frame, update stamp
    // lastcb.clear();
    // for (int p = 0; p < msg_list.size(); p++) 
    //   lastcb.push_back(msg_list[p]);
    // lasttime = msg->image_header.stamp;
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
void getPoint(const sensor_msgs::PointCloud2::ConstPtr& msg) {
	 //ROS_INFO("get point");
    sensor_msgs::PointCloud2 cop = *msg;
    if (buf_index >= 200 || buf_index < 0) {
      buf_index = 0;
    }
    pc_buffer[buf_index] = *msg;
    buf_index++;
    //ROS_INFO("got point");
}


int main(int argc, char**argv) {
  ros::init(argc, argv, "move_to_location");
  ros::NodeHandle moveNode;

  ros::Subscriber mapSub = moveNode.subscribe("/level_mux/map", 1, getMapId);
  ros::Subscriber darknetSub = moveNode.subscribe("/darknet_ros/bounding_boxes", 1, darkcb);
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
  location_list.push_back(d414a);
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
  location_index++;
  while (ros::ok())
  {
	  if (0 != rm && !rm->action_status())
	  {
		  //initial loop
		  if (!drive) {
			  ROS_INFO("Moving to location %d", location_index);
			  rm->move_to_pose(location_list[location_index]);
			  location_index++;
				if (location_index >= location_list.size()) {
					location_index = 0;
				}
		  } else {
			  moveCircle(targ, targclass);
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
