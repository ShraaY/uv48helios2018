#include "ros/ros.h"
#include "geometry_msgs/Quaternion.h"
#include "std_msgs/Header.h"
#include <vector>
#include "geometry_msgs/PoseStamped.h"

int main(int argc,char** argv){
    ros::init(argc, argv, "VectX_test");
    ros::NodeHandle n;

    ros::Publisher chatter_pub = n.advertise<geometry_msgs::PoseStamped>("Pose_vect_X", 1);
    ros::Rate loop_rate(20);


    while(ros::ok()){
      //creation vetX
        geometry_msgs::PoseStamped msg;
        msg.pose.position.x = 7;
        msg.pose.position.y = 6;

        chatter_pub.publish(msg);

        ros::spinOnce();
        loop_rate.sleep();
    }



    return 0;
}
