#include <iostream>                                                                                     
#include <vector>                                                                                     

//Boost includes
#include <boost/shared_ptr.hpp>

//Ros includes
#include <ros/ros.h>
#include <rws2018_libs/team.h>

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
        cout << "cannot set team name to " << argin_team << endl;
        return 0;
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

  MyPlayer(string argin_name, string argin_team) : Player(argin_name)
  {
    red_team = boost::shared_ptr<Team> (new Team("red"));
    green_team = boost::shared_ptr<Team> (new Team("green"));
    blue_team = boost::shared_ptr<Team> (new Team("blue"));

    setTeamName(argin_team);


    printReport();
  }

  void printReport()
  {
    cout << "My name is " << name << " and my team is " << getTeamName() << endl;
  }

};

}//end of namespace

int main(int argc, char** argv)
{

  ros::init(argc, argv, "moliveira");

  //Creating an instance of class Player
  rws_moliveira::MyPlayer my_player("moliveira", "green");

  if (my_player.red_team->playerBelongsToTeam("amartins"))
  {
    cout << "a joana esta na equipa certa" << endl;
  };

  ros::NodeHandle n;

  //vector<string> list_of_players;
  //string test_param_value;
  //n.getParam("test_param", test_param_value);

  //cout << "read test_param with value " << test_param_value << endl;

  ros::spin();

}

