#include <iostream>   
#include <vector>                                                                                     

//Boost includes
#include <boost/shared_ptr.hpp>

//Ros includes
#include <ros/ros.h>
#include <rws2018_libs/team.h>
#include <tf/transform_broadcaster.h>

#include <rws2018_msgs/MakeAPlay.h>

using namespace std;

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

      struct timeval t1;
      gettimeofday(&t1, NULL);
      srand(t1.tv_usec);
      double start_x = ((double)rand()/(double)RAND_MAX) *10 -5;
      double start_y = ((double)rand()/(double)RAND_MAX) *10 -5;
      printf("start_x=%f start_y=%f\n", start_x, start_y);
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

      void move(const rws2018_msgs::MakeAPlay::ConstPtr& msg)
      {
        double x = T.getOrigin().x();
        double y = T.getOrigin().y();
        double a = 0;

        T.setOrigin( tf::Vector3(x+=0.01, y, 0.0) );
        tf::Quaternion q;
        q.setRPY(0, 0, a);
        T.setRotation(q);

        br.sendTransform(tf::StampedTransform(T, ros::Time::now(), "world", "moliveira"));
        ROS_INFO("Moving to ");

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

