#include "ros/ros.h"
#include "nav_msgs/Path.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Quaternion.h"
#include "std_msgs/Header.h"
#include <vector>

int main(int argc,char** argv){
    ros::init(argc, argv, "Path_test");
    ros::NodeHandle n;

    ros::Publisher chatter_pub = n.advertise<nav_msgs::Path>("Path_WayPoint", 1);
    ros::Rate loop_rate(20);


    while(ros::ok()){
      //creation path |_|-|
        nav_msgs::Path msg;
        geometry_msgs::PoseStamped a,b,c,d,e,f;
        a.pose.position.x = 0;
        a.pose.position.y = 1;

        b.pose.position.x = 0;
        b.pose.position.y = 4;

        c.pose.position.x = 2;
        c.pose.position.y = 4;

        d.pose.position.x = 2;
        d.pose.position.y = 1;

        e.pose.position.x = 4;
        e.pose.position.y = 1;

        f.pose.position.x = 4;
        f.pose.position.y = 4;
        std::vector<geometry_msgs::PoseStamped> envoi;
        envoi.push_back(a);
        envoi.push_back(b);
        envoi.push_back(c);
        envoi.push_back(d);
        envoi.push_back(e);
        envoi.push_back(f);
        msg.poses.resize(envoi.size());
        for(unsigned int i=0; i < envoi.size(); i++){
            msg.poses[i] = envoi[i];
        }
        chatter_pub.publish(msg);

        ros::spinOnce();
        loop_rate.sleep();
    }



    return 0;
}
