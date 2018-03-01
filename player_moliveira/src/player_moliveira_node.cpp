#include <iostream>                                                                                     

class Player
{
  public:
    Player(std::string argin_name)
    {
      name = argin_name;
    }

    std::string name;

    //Overloaded setter for team_nam
    int setTeamName(int index = 2)
    {
      if (index == 0){ setTeamName("red");}
      else if (index == 1){ setTeamName("green");}
      else if (index == 2){ setTeamName("blue");}
      else { setTeamName("none");}
    }

    //Setter for team_nam
    int setTeamName(std::string argin_team)
    {
      if (argin_team=="red" || argin_team=="green" || argin_team=="blue")
      {
        team_name = argin_team; return 1;
      }
      else
      {
        std::cout << "cannot set team name to " << argin_team << std::endl;
        return 0;
      }
    }

    //Getter of team_name
    std::string getTeamName(void)
    {
      return team_name;
    }

  private:
    std::string team_name;
};


class MyPlayer : public Player
{
  public:
  MyPlayer(std::string argin_name, std::string argin_team) : Player(argin_name)
  {
    setTeamName(argin_team);
  }

};


int main()
{
  //Creating an instance of class Player
  Player player("moliveira");

  std::cout << "Created an instance of class player with public name " << player.name << std::endl;
  player.setTeamName();
  std::cout << "team_name = " << player.getTeamName() << std::endl;

  MyPlayer my_player("moliveira", "green");

  std::cout << "myplayer team_name = " << my_player.getTeamName() << std::endl;
}

