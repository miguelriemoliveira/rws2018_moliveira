//#include <iostream>
//#include <boost/thread/thread.hpp>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <ros/package.h>
//#include <rospack/rospack.h>
#include <sensor_msgs/PointCloud2.h>

#include <visualization_msgs/Marker.h>

#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>


using namespace std;
using namespace sensor_msgs;
typedef pcl::PointXYZRGB PointT;
pcl::PointCloud<PointT>::Ptr cloud(new pcl::PointCloud<PointT>);

int main (int argc, char** argv)
{
    ros::init(argc, argv, "pcd2pointcloud", ros::init_options::AnonymousName);
    ros::NodeHandle nh;


    cloud = (pcl::PointCloud<PointT>::Ptr ) new pcl::PointCloud<PointT>;

    string input; 
    if (ros::param::get("~input", input))
    {
        ROS_INFO("Loading point cloud from file %s",input.c_str());
        if (pcl::io::loadPCDFile<PointT>(input.c_str(), *cloud) != 0)
        {
            return -1;
        }
        cout << "Cloud has " << cloud->points.size () << " data points!" << endl;
    }
    else
    {
        ROS_ERROR("No point cloud filename given as input. use _input:=<filename>");
        exit(1);
    }

    int one_shot;
    ros::param::get("~one_shot", one_shot);
    printf("one_shot=%d\n", one_shot);
    if (one_shot)
    {
        cout << "One shot pcd2pointcloud" << endl;
    }
    else
    {
        cout << "Continuous pcd2pointcloud" << endl;
    }

    sensor_msgs::PointCloud2 msg;
    pcl::toROSMsg (*cloud, msg);
    msg.header.frame_id = ros::names::remap("/world");

    string output="/camera/depth_registered/points"; 
    if (ros::param::get("~output", output))
    {
        ROS_INFO("Publishing point cloud on topic %s", output.c_str());
    }
    else
    {
        ROS_WARN("No topic name given, using default %s",output.c_str());
    }

    ros::Publisher pub = nh.advertise<sensor_msgs::PointCloud2>(output, 1);


    //Create the visualization msg
    ros::Publisher pub_marker = nh.advertise<visualization_msgs::Marker>(output + "_marker", 1);

    visualization_msgs::Marker marker;
    marker.header.frame_id = ros::names::remap("/world");
    marker.header.stamp = ros::Time();
    marker.ns = ros::names::remap("/world");
    marker.id = 0;
    marker.type = visualization_msgs::Marker::POINTS;
    marker.action = visualization_msgs::Marker::ADD;
    marker.scale.x = 0.1;
    marker.scale.y = 0.1;
    marker.color.a = 1.0; // Don't forget to set the alpha!
    //marker.color.r = 1.0; // Don't forget to set the alpha!
    marker.lifetime = ros::Duration(4); // Don't forget to set the alpha!
    marker.frame_locked = 1; // Don't forget to set the alpha!

    for (size_t i=0; i < cloud->points.size(); ++i)
    {
        geometry_msgs::Point p;
        p.x = cloud->points[i].x;
        p.y = cloud->points[i].y;
        p.z = cloud->points[i].z;
        marker.points.push_back(p);

        std_msgs::ColorRGBA c;
        c.r = cloud->points[i].r;
        c.g = cloud->points[i].g;
        c.b = cloud->points[i].b;
        c.a = 1.0;
        marker.colors.push_back(c);
    }

    ros::Rate loop_rate(5);
    ros::spinOnce();
    ros::Time tic = ros::Time::now();
    while (ros::ok())
    {
        loop_rate.sleep();
        pub.publish(msg);
        marker.header.stamp = ros::Time::now();
        pub_marker.publish(marker);

        ros::spinOnce();

        if (one_shot)
        {
            if ((ros::Time::now() - tic).toSec() > 1)
            {
                break;
            }
        }
    }

    return (0);
}


