#include "ros/ros.h"
#include "std_msgs/String.h"

#include <tf/transform_broadcaster.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
//#include <rws2016_libs/team_info.h>

ros::Publisher marker_pub;

using namespace std;

string myTeamId;
string myHuntersId;
string myPreysId;



int main(int argc, char **argv)
{
    ros::init(argc, argv, "draw_teams");
    ros::NodeHandle node;

    //team_info(node, myTeam, myHunters, myPreys, myTeamId, myHuntersId, myPreysId);


    marker_pub = node.advertise<visualization_msgs::MarkerArray>("player_markers", 1);

    vector<string> teamA, teamB, teamC;
    double hunting_distance;
    node.getParam("/team_red", teamA);
    node.getParam("/team_green", teamB);
    node.getParam("/team_blue", teamC);
    node.getParam("/hunting_distance", hunting_distance);
    hunting_distance = hunting_distance*2;

    visualization_msgs::MarkerArray ma;

    size_t count=0;

    visualization_msgs::Marker marker;
    marker.header.stamp = ros::Time();
    marker.ns = "ns";
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.x = 0;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.frame_locked = true;
    marker.lifetime = ros::Duration(0);

    for (size_t i=0; i<teamA.size(); ++i)
    {
        //Draw an arrow
        marker.header.frame_id = teamA[i];
        marker.id = count++;
        marker.type = visualization_msgs::Marker::CYLINDER;
        marker.pose.position.z = -0.2;
        marker.scale.x = hunting_distance; marker.scale.y = hunting_distance; marker.scale.z = 0.01;
        marker.color.a = 0.2; // Don't forget to set the alpha!
        marker.color.r = 1.0; marker.color.g = 1.0; marker.color.b = 1.0;
        ma.markers.push_back(marker);

        //Draw a circle
        marker.id = count++;
        marker.type = visualization_msgs::Marker::ARROW;
        marker.scale.x = 0.5; marker.scale.y = 0.1; marker.scale.z = 0.1;
        marker.color.a = 0.7; // Don't forget to set the alpha!
        marker.color.r = 1.0; marker.color.g = 0.0; marker.color.b = 0.0;
        ma.markers.push_back(marker);

        //Draw text
        marker.id = count++;
        marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
        marker.scale.x = 0.8; marker.scale.y = 0.2; marker.scale.z = 0.5;
        marker.color.a = 1.0; // Don't forget to set the alpha!
        marker.color.r = 1.0; marker.color.g = 0.0; marker.color.b = 0.0;

        marker.text = teamA[i];

        ma.markers.push_back(marker);

    }

    for (size_t i=0; i<teamB.size(); ++i)
    {
        //Draw an arrow
        marker.header.frame_id = teamB[i];
        marker.id = count++;
        marker.type = visualization_msgs::Marker::CYLINDER;
        marker.pose.position.z = -0.2;
        marker.scale.x = hunting_distance; marker.scale.y = hunting_distance; marker.scale.z = 0.01;
        marker.color.a = 0.2; // Don't forget to set the alpha!
        marker.color.r = 1.0; marker.color.g = 1.0; marker.color.b = 1.0;
        ma.markers.push_back(marker);

        //Draw a circle
        marker.id = count++;
        marker.type = visualization_msgs::Marker::ARROW;
        marker.scale.x = 0.5; marker.scale.y = 0.1; marker.scale.z = 0.1;
        marker.color.a = 0.7; // Don't forget to set the alpha!
        marker.color.r = 0.0; marker.color.g = 0.6; marker.color.b = 0.0;
        ma.markers.push_back(marker);

        //Draw text
        marker.id = count++;
        marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
        marker.scale.x = 0.8; marker.scale.y = 0.2; marker.scale.z = 0.5;
        marker.color.a = 1.0; // Don't forget to set the alpha!
        marker.color.r = 0.0; marker.color.g = 0.6; marker.color.b = 0.0;
        marker.text = teamB[i];
        ma.markers.push_back(marker);

    }

    for (size_t i=0; i<teamC.size(); ++i)
    {
        //Draw an arrow
        marker.header.frame_id = teamC[i];
        marker.id = count++;
        marker.type = visualization_msgs::Marker::CYLINDER;
        marker.pose.position.z = -0.2;
        marker.scale.x = hunting_distance; marker.scale.y = hunting_distance; marker.scale.z = 0.01;
        marker.color.a = 0.2; // Don't forget to set the alpha!
        marker.color.r = 1.0; marker.color.g = 1.0; marker.color.b = 1.0;
        ma.markers.push_back(marker);

        //Draw a circle
        marker.id = count++;
        marker.type = visualization_msgs::Marker::ARROW;
        marker.scale.x = 0.5; marker.scale.y = 0.1; marker.scale.z = 0.1;
        marker.color.a = 0.7; // Don't forget to set the alpha!
        marker.color.r = 0.0; marker.color.g = 0.0; marker.color.b = 1.0;
        ma.markers.push_back(marker);

        //Draw text
        marker.id = count++;
        marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
        marker.scale.x = 0.8; marker.scale.y = 0.2; marker.scale.z = 0.5;
        marker.color.a = 1.0; // Don't forget to set the alpha!
        marker.color.r = 0.0; marker.color.g = 0.0; marker.color.b = 1.0;
        marker.text = teamC[i];
        ma.markers.push_back(marker);
    }


    ros::Rate loop_rate(10);

    while (ros::ok())
    {
        marker_pub.publish(ma);
        loop_rate.sleep();
    }

    return 0;
}
