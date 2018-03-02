#ifndef _TEAM_H_
#define _TEAM_H_

#include <iostream>
#include <vector>

// Boost includes
#include <boost/shared_ptr.hpp>

// Ros includes
#include <ros/ros.h>

float randomizePosition()
{
  return (((double)rand() / (RAND_MAX)) - 0.5) * 10;
}

class Team
{
public:
  Team(std::string name)
  {
    team_name = name;
    ros::NodeHandle n;
    n.getParam("/team_" + name, player_names);

    ROS_INFO_STREAM("I am team  " << name << " and my players are:");
    for (size_t i = 0; i < player_names.size(); i++)
    {
      ROS_INFO_STREAM(player_names[i]);
    }
  }

  bool playerBelongsToTeam(std::string player_name)
  {
    for (size_t i = 0; i < player_names.size(); i++)
    {
      if (player_name == player_names[i])
        return true;
    }
    return false;
  }

  std::vector<std::string> player_names;
  std::string team_name;
};

#endif
