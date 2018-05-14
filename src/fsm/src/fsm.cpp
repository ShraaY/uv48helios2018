#include "ros/ros.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>

#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/PointStamped.h"
#include <geometry_msgs/TransformStamped.h>
#include "geometry_msgs/PoseArray.h"
#include "nav_msgs/Path.h"

#include "std_msgs/Float64.h"
#include "std_msgs/Int64.h"
#include "std_msgs/String.h"
#include "std_msgs/Bool.h"

#include "tf/tf.h"
#include <tf/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>


#define ERR_DISTANCE 5


std::vector<geometry_msgs::PoseStamped> msg_waypoints;
std::array<float, 2> a = { 0, 0 };
std::array<float, 2> b = { 0, 0 };
std::array<float, 2> helios = { 0, 0 };
bool isOkay = 1;
std::string etat_commande;
std_msgs::String msg_etat;


// fonctions appelées par les subscribers
void commandeCallback(const nav_msgs::Path::ConstPtr& msg_path){
  //recupere les waypoints (matrice 2xn)
 msg_waypoints = msg_path->poses;
}

void vectXCallback(const geometry_msgs::PoseStamped::ConstPtr& msg_helios){
  //recupere la position en x et y du bateau
  helios[0] = msg_helios->pose.position.x;
  helios[1] = msg_helios->pose.position.y;
}

void isOkayCallback(const std_msgs::Bool::ConstPtr& msg_isOkay){
  //recupere l'etat ok ou non de la commande
  isOkay = msg_isOkay->data;
}

void EtatCallback(const std_msgs::String::ConstPtr& msg_etat_commande){
  //recupere le string du l'état voulu par la commande
  etat_commande = msg_etat_commande->data;
}


int main(int argc, char **argv){

    int lenght_wpts;
    float err;  //erreur de distance
    int i = 0;  //waypoint courant
    int j = 0;

    ros::init(argc, argv, "FSM");
    ros::NodeHandle n;

    //Ecoute sur
    ros::Subscriber vectX_sub = n.subscribe("Pose_vect_X", 1, vectXCallback);
    ros::Subscriber commande_sub = n.subscribe("Path_WayPoint", 1, commandeCallback);
    ros::Subscriber is_okay_sub = n.subscribe("Bool_is_OK", 1, isOkayCallback);
    ros::Subscriber etat_sub = n.subscribe("String_Etat", 1, EtatCallback);

    //Publie sur
    ros::Publisher etats_pub = n.advertise<std_msgs::String>("String_Etat_Retour", 1);
    ros::Publisher direction_pub = n.advertise<geometry_msgs::PoseArray>("Path_Direction", 1);
    ros::Publisher num_wpt_pub = n.advertise<std_msgs::Int64>("Numero", 1);

    ros::Rate loop_rate(20);

    std_msgs::Int64 msg_num_wpts;
    msg_num_wpts.data = 0;
    std::string etat_precedant = "Idle";
    msg_etat.data = "Idle"; //fsm state debut : Idle

    while(ros::ok()){

      if (isOkay == 1){
        lenght_wpts = msg_waypoints.size();

        //changement d'etat FSM
        if (etat_precedant == "Idle" && etat_commande == "Mission"){
          msg_etat.data = "Mission";
          etat_precedant = "Mission";

        }else if (etat_precedant == "Sleep"){
          if (etat_commande == "Reset"){
            msg_etat.data = "Reset";
            etat_precedant = "Reset";
          }else if (etat_commande == "Mission"){
            msg_etat.data = "Mission";
            etat_precedant = "Mission";
          }

        }else if (etat_precedant == "Reset" && etat_commande == "Idle"){
            msg_etat.data = "Idle";
            etat_precedant = "Idle";

        }else if (etat_precedant == "Mission"){
          if (etat_commande == "Sleep"){
            msg_etat.data = "Sleep";
            etat_precedant = "Sleep";
          }else if (etat_commande == "Reset"){
            msg_etat.data = "Reset";
            etat_precedant = "Reset";
          }

          //???????
          //if (i >= lenght_wpts) { //pas de waypoints disponibles
           // msg_etat.data = "attente_points"; //fsm state Sleep???
          //}

        }
        //Publication des messages
        etats_pub.publish(msg_etat);

        //en mode Reset : recommence depuis le debut de la liste de waypoints
        if (msg_etat.data == "Reset"){
          i = 0;
        }//fin if mode Reset

        //en mode Mission
        if (msg_etat.data == "Mission"){

          if (lenght_wpts >= 2){

            //recuperation des coordonnées pour le suivi de ligne
            a[0] = msg_waypoints[i].pose.position.x;
            a[1] = msg_waypoints[i].pose.position.y;
            b[0] = msg_waypoints[i+1].pose.position.x;
            b[1] = msg_waypoints[i+1].pose.position.y;
            //printf("a : %f, %f | b : %f, %f\n", a[0], a[1], b[0], b[1]);

            //creation du message pour le suivi de ligne : [[a], [b]]
            geometry_msgs::PoseArray msg_ab;
            msg_ab.header.stamp = ros::Time::now();

            for (j = 0 ; j < 2; j++){
              geometry_msgs::Pose tmp;
              tmp.position.x = a[0];
              tmp.position.y = a[1];
              msg_ab.poses.push_back(tmp);
            }
            msg_ab.poses[1].position.x = b[0];
            msg_ab.poses[1].position.y = b[1];

            //calcul de la distance au waypoint
            err = (b[0] - a[0])*(helios[1] - a[1]) -
                (b[1] - a[1])*(helios[0] - a[0]);

            //passage au pt suivant
            if (err < ERR_DISTANCE){
              i++;
            }

            //creation du message donnant le numero du waypoint visé
            std_msgs::Int64 msg_num_wpts;
            msg_num_wpts.data = i+1;

            //Publication des messages
            direction_pub.publish(msg_ab);
            num_wpt_pub.publish(msg_num_wpts);

          }// fin if taille tableau <=2
        } //fin if mode Mission

      } //fin if isOkay

      ros::spinOnce();
      loop_rate.sleep();

    } // fin while
    return 0;
}
