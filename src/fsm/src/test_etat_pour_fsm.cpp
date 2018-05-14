#include "ros/ros.h"
#include "std_msgs/String.h"

int main(int argc,char** argv){
    ros::init(argc, argv, "Etat_test");
    ros::NodeHandle n;

    ros::Publisher etat_pub = n.advertise<std_msgs::String>("String_Etat", 1);
    ros::Rate loop_rate(20);

    while(ros::ok()){
      //creation msg
      std_msgs::String msg;
      msg.data = "Mission";
      etat_pub.publish(msg);

      ros::spinOnce();
      loop_rate.sleep();
    }
    return 0;
}
