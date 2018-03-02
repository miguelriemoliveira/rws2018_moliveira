#include <iostream>   
#include <vector>                                                                                     

//Boost includes
#include <boost/shared_ptr.hpp>

//Ros includes
#include <ros/ros.h>
#include <rws2018_libs/team.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <visualization_msgs/Marker.h>

#include <rws2018_msgs/MakeAPlay.h>

#define DEFAULT_TIME 0.05

using namespace std;
using namespace ros;
using namespace tf;

namespace rws_moliveira
{

  class Player
  {
    public:
      Player(string argin_name) {name = argin_name;}

      string name;

      //Overloaded setter for team_nam
      int setTeamName(int index = 2)
      {
        if (index == 0){ setTeamName("red");}
        else if (index == 1){ setTeamName("green");}
        else if (index == 2){ setTeamName("blue");}
        else { setTeamName("none");}
      }

      //Setter for team_nam
      int setTeamName(string argin_team)
      {
        if (argin_team=="red" || argin_team=="green" || argin_team=="blue")
        {
          team_name = argin_team; return 1;
        }
        else
        {
          ROS_ERROR("cannot set team name to %s", argin_team.c_str());
          ros::shutdown();
        }
      }

      //Getter of team_name
      string getTeamName(void)
      {
        return team_name;
      }

    private:
      string team_name;
  };


  class MyPlayer : public Player
  {
    public:

      boost::shared_ptr<Team> red_team;
      boost::shared_ptr<Team> green_team;
      boost::shared_ptr<Team> blue_team;
      boost::shared_ptr<Team> my_team;
      boost::shared_ptr<Team> my_preys;
      boost::shared_ptr<Team> my_hunters;

      tf::TransformBroadcaster br; //declare the broadcaster
      ros::NodeHandle n;
      boost::shared_ptr<ros::Subscriber> sub;
      tf::Transform T; //declare the transformation object (player's pose wrt world)
      boost::shared_ptr<ros::Publisher> pub;
      tf::TransformListener listener;

      MyPlayer(string argin_name, string argin_team/*disregard this one. overrided by params*/) : Player(argin_name)
    {
      red_team = boost::shared_ptr<Team> (new Team("red"));
      green_team = boost::shared_ptr<Team> (new Team("green"));
      blue_team = boost::shared_ptr<Team> (new Team("blue"));

      if (red_team->playerBelongsToTeam(name))
      {
        my_team = red_team;
        my_preys = green_team;
        my_hunters = blue_team;
        setTeamName("red");
      }
      else if (green_team->playerBelongsToTeam(name))
      {
        my_team = green_team;
        my_preys = blue_team;
        my_hunters = red_team;
        setTeamName("green");
      }
      else if (blue_team->playerBelongsToTeam(name))
      {
        my_team = blue_team;
        my_preys = red_team;
        my_hunters = green_team;
        setTeamName("blue");
      }

      sub = boost::shared_ptr<ros::Subscriber> (new ros::Subscriber());
      *sub = n.subscribe("/make_a_play", 100, &MyPlayer::move, this);

      pub = boost::shared_ptr<ros::Publisher> (new ros::Publisher());
      *pub = n.advertise<visualization_msgs::Marker>( "/bocas", 0 );

      struct timeval t1;
      gettimeofday(&t1, NULL);
      srand(t1.tv_usec);
      double start_x = ((double)rand()/(double)RAND_MAX) *10 -5;
      double start_y = ((double)rand()/(double)RAND_MAX) *10 -5;
      printf("start_x=%f start_y=%f\n", start_x, start_y);

      ros::Duration(0.1).sleep();
      warp(start_x, start_y, M_PI/2);

      printReport();
    }

      void warp(double x, double y, double alfa)
      {
        T.setOrigin( tf::Vector3(x, y, 0.0) );
        tf::Quaternion q;
        q.setRPY(0, 0, alfa);
        T.setRotation(q);
        br.sendTransform(tf::StampedTransform(T, ros::Time::now(), "world", "moliveira"));
        ROS_INFO("Warping to x=%f y=%f a=%f", x,y,alfa);
      }


      double getDistanceToPlayer(string other_player, double time_to_wait=DEFAULT_TIME)
      {
        StampedTransform t; //The transform object
        //Time now = Time::now(); //get the time
        Time now = Time(0); //get the latest transform received

        try{
          listener.waitForTransform("moliveira", other_player, now, Duration(time_to_wait));
          listener.lookupTransform("moliveira", other_player, now, t);
        }
        catch (TransformException& ex){
          ROS_ERROR("%s",ex.what());
          return NAN;
        }

        return sqrt(t.getOrigin().y()*t.getOrigin().y() + t.getOrigin().x()*t.getOrigin().x());
      }

      double getAngleToPLayer(string other_player, double time_to_wait=DEFAULT_TIME)
      {
        StampedTransform t; //The transform object
        //Time now = Time::now(); //get the time
        Time now = Time(0); //get the latest transform received

        try{
          listener.waitForTransform("moliveira", other_player, now, Duration(time_to_wait));
          listener.lookupTransform("moliveira", other_player, now, t);
        }
        catch (TransformException& ex){
          ROS_ERROR("%s",ex.what());
          return NAN;
        }

        return atan2(t.getOrigin().y(), t.getOrigin().x());
      }


      void move(const rws2018_msgs::MakeAPlay::ConstPtr& msg)
      {
        double x = T.getOrigin().x();
        double y = T.getOrigin().y();
        double a = 0;

        //---------------------------------------
        //--- AI PART 
        //---------------------------------------
        
        //Find nearest prey (player_to_hunt will be the nearest prey player)
        double min_distance = 99999;
        string player_to_hunt = "no player";
        for (size_t i=0; i < msg->green_alive.size(); i++)
        {
            double dist = getDistanceToPlayer(msg->green_alive[i]);
            if (isnan(dist))
            {
            }
            else if (dist < min_distance)
            {
              min_distance = dist;
              player_to_hunt = msg->green_alive[i];
            }
        }
        
        double displacement = 1; //max velocity for now
        double delta_alpha = getAngleToPLayer(player_to_hunt);
        if (isnan(delta_alpha))
          delta_alpha = 0;

        visualization_msgs::Marker marker;
        marker.header.frame_id = "moliveira";
        marker.header.stamp = ros::Time();
        marker.ns = "moliveira";
        marker.id = 0;
        marker.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.orientation.w = 1.0;
        marker.scale.z = 0.3;
        marker.color.a = 1.0; // Don't forget to set the alpha!
        marker.color.r = 1.0;
        marker.color.g = 1.0;
        marker.color.b = 0.0;
        marker.text = "vou-te apanhar " + player_to_hunt;
        marker.lifetime = ros::Duration(2);
        pub->publish( marker );

        //---------------------------------------
        //--- CONSTRAINS PART 
        //---------------------------------------
        double displacement_max = msg->dog;
        displacement > displacement_max ? displacement = displacement_max: displacement = displacement;

        double delta_alpha_max = M_PI/30;
        fabs(delta_alpha) > fabs(delta_alpha_max) ? delta_alpha = delta_alpha_max * delta_alpha / fabs(delta_alpha): delta_alpha = delta_alpha;

        tf::Transform my_move_T; //declare the transformation object (player's pose wrt world)
        my_move_T.setOrigin( tf::Vector3(displacement, 0.0, 0.0) );
        tf::Quaternion q1;
        q1.setRPY(0, 0, delta_alpha);
        my_move_T.setRotation(q1);

        T = T * my_move_T;
        br.sendTransform(tf::StampedTransform(T, ros::Time::now(), "world", "moliveira"));

      }

      void printReport()
      {
        ROS_INFO("My name is %s and my team is %s", name.c_str(), (getTeamName().c_str()) );
      }

  };

}//end of namespace

int main(int argc, char** argv)
{

  ros::init(argc, argv, "moliveira");
  ros::NodeHandle n;



  //Creating an instance of class Player
  rws_moliveira::MyPlayer my_player("moliveira", "doesnotmatter");

  ros::spin();
  //ros::Rate loop_rate(10);
  //while (ros::ok())
  //{
  //my_player.move();

  //ros::spinOnce();
  //loop_rate.sleep();
  //}

}

