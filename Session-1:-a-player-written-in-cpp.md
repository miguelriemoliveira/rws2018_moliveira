This small introduction to c++ is based on the work of Jorge Almeida.
Check his pdf for additional information.

Some interesting links:

[http://www.cplusplus.com/doc/tutorial/](http://www.cplusplus.com/doc/tutorial/)

[https://class.coursera.org/cplusplus4c-002/lecture](https://class.coursera.org/cplusplus4c-002/lecture)

We will use c++ functionalities to create code for our game. Later on we will plug in the ROS stuff to this code skeleton.

first lets create a file:

    cd ~/catkin_ws/src/rwsua2017/rwsua2017_moliveira/player_moliveira/src
    touch player_moliveira_node.cpp

and paste the following inside the file:

```cpp
#include <iostream>
    
int main()
{
    std::cout << "Hello world" << std::endl;           
    return 1;                                                      
}
```

Editing _CMakeLists.txt_, compiling with _catkin_make_ and executing with _rosrun_

# Classes, constructor and public atributes

Create a class _Player_, with a constructor which receives as argument an _std::string_.

```cpp
#include <iostream>                                                                                     

class Player
{
    public:

    Player(std::string name)
    {
        this->name = name;
    }


    std::string name;

    private:

};

int main()
{

    std::string player_name = "moliveira";
    //Creating an instance of class Player
    Player player(player_name);

    std::cout << "Created an instance of class player with public name " << player.name << std::endl;
}
                                                                                   
```

# Private attributes and accessors

```cpp
#include <iostream>

class Player
{
    public:

    //Constructor with the same name as the class
    Player(std::string name) { this->name = name;}

    //Set team name, if given a correct team name (accessor)
    int setTeamName(std::string team)
    {
        if (team=="red" || team=="green" || team=="blue")
        {
            this->team = team;
            return 1;
        }
        else
        {
            std::cout << "cannot set team name to " << team << std::endl;
            return 0;
        }
    }

    //Gets team name (accessor)
    std::string getTeam(void) {return team;}
    
    std::string name; //A public atribute

    private:
    std::string team;
};

int main()
{
    //Creating an instance of class Player
    Player player("moliveira");
    player.setTeamName("red");
                                                                                                        
    std::cout << "player.name is " << player.name << std::endl;
    std::cout << "team is " << player.getTeam() << std::endl;
}

```

## Function overload and function argument default values

```cpp
#include <iostream>

class Player
{
    public:

    //Constructor with the same name as the class
    Player(std::string name) { this->name = name;}

    int setTeamName(int team_index = 0 /*default value*/)
    {
        switch (team_index)
        {
            case 0: 
                return setTeamName("red"); break;
            case 1: 
                return setTeamName("green"); break;
            case 2: 
                return setTeamName("blue");  break;
            default: 
                std::cout << "wrong team index given. Cannot set team" << std::endl; break;
        }
    }

    //Set team name, if given a correct team name (accessor)
    int setTeamName(std::string team)
    {
        if (team=="red" || team=="green" || team=="blue")
        {
            this->team = team;
            return 1;                                                                                   
        }
        else
        {
            std::cout << "cannot set team name to " << team << std::endl;
            return 0;
        }
    }

    //Gets team name (accessor)
    std::string getTeamName(void) {return team;}
    
    std::string name; //A public atribute

    private:
    std::string team;
};

int main()
{
    //Creating an instance of class Player
    Player player("moliveira");
    player.setTeamName("red");
    player.setTeamName(2);
    std::cout << "player.name is " << player.name << std::endl;
    std::cout << "team is " << player.getTeamName() << std::endl;

}
```

# Class inheritance, passing arguments to superclass constructor

```cpp

class Player 
// ... (hidden for better visualization)

//Class myPlayer extends class Player
class myPlayer: public Player
{
    public: 
                                                                                                        
    myPlayer(std::string name, std::string team): Player(name)
    {
        setTeamName(team);
    }
};

int main()
{
    //Creating an instance of class Player
    myPlayer my_player("moliveira","green");
    std::cout << "my_player.name is " << my_player.name << std::endl;
    std::cout << "team is " << my_player.getTeamName() << std::endl;
}


```

# Namespaces

We can use namespaces to group functions, classes, variables, etc under a common name.

For example, all opencv stuff is under the namespace _cv::_

In the following code, we created our own namespace. In addition, we are using a _shortcut_ to the namespace _std_

```cpp

#include <iostream>

using namespace std;                                                                                    

namespace rwsua2017_moliveira
{

class Player /*hidden for better visualization*/

//Class myPlayer extends class Player
class myPlayer: public Player /*hidden for better visualization*/

} //end of namespace rwsua2017_moliveira
                                                                                                        
int main()
{
    //Creating an instance of class Player
    rwsua2017_moliveira::myPlayer my_player("moliveira","green");
    cout << "my_player.name is " << my_player.name << endl;
    cout << "team is " << my_player.getTeamName() << endl;                                              
}
```

# _std::vector_, _boost::shared_ptr_, templates and references

A class team should contain a list of players.

```cpp

#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>

using namespace std;                                                                                    

namespace rwsua2017_moliveira
{

class Player /*hidden for better visualization*/

//Class myPlayer extends class Player
class myPlayer: public Player /*hidden for better visualization*/

class Team
{
    public: 

    //Team constructor
    Team(string team, vector<string>& player_names)
    {
       name = team; 

       //Cycle all player names, and create a class player for each
       for (size_t i=0; i < player_names.size(); ++i)
       {
           //Why? Copy constructable ...
           boost::shared_ptr<Player> p(new Player(player_names[i]));
           p->setTeamName(name);
           players.push_back(p);
       }
    
    }

    void printTeamInfo(void)
    {
        cout << "Team " << name << " has the following players:" << endl;

        for (size_t i=0; i < players.size(); ++i)
            cout << players[i]->name << endl;
    }
    
    string name;
    vector<boost::shared_ptr<Player> > players;
};  

} //end of namespace rwsua2017_moliveira

int main()
{
    //Creating an instance of class Player
    vector<string> players;
    players.push_back("moliveira");
    players.push_back("vsantos");
    players.push_back("pdias");

    rwsua2017_moliveira::Team team("green", players);
    team.printTeamInfo();
} 
    
```