#Select your team 

[Here](https://github.com/miguelriemoliveira/rwsua2017_moliveira/wiki/Teams)

# Introduction

Instructions for the Ros Workshop 2017.
Miguel Oliveira, Vitor Santos.
March 2017.

In this workshop each participant will develop his own software module. Each module represents a player which will play a game according to the code written by the participant. 

The game is called **Team Hunt**. Check [here](https://github.com/miguelriemoliveira/rwsua2017_moliveira/wiki/Game-description) for a description of the game 

# What we will learn to use from ROS

* Creating ROS packages, nodes and libraries, compiling and
running them
* Using ROS based communications, publish / subscribe, server
/ response
* Using ROS launch file scripts to ease the startup of complex
systems
* Using ros parameters to configure the nodes
* Creating custom messages
* Using RVIZ and publishing visualization markers
* Visualizing the ROS nodes and the topics they are exchanging
* Using the ROS tf library (just the basics)
* Using the rosbag tool to record the system’s output

# Prerequisites

You must first make sure you have all the [prerequisites](https://github.com/miguelriemoliveira/rwsua2017_moliveira/wiki/Prerequisites) completed.

# Create a folder for the workshop

In you catkin workspace, create a folder which will contain all the code for the workshop

    mkdir ~/catkin_ws/src/rwsua2017

# Create a git hub repository and clone
Go to

    https://github.com/

and sign in (register if you have not before). Create your first repository, naming of the repository should be: 

    rws2016_rwsua2017_<f><lastname>

if you are called **f**irstname **lastname**. 
In my case, Miguel Oliveira, I created the repository rwsua2017_moliveira.

Now you can clone the repo (check in your github page the url of your repo)
   
    cd ~/catkin_ws/src/rwsua2017
    git clone https://github.com/miguelriemoliveira/rwsua2017_moliveira.git

# Create a ROS package inside the repo

    cd ~/catkin_ws/src/rws2017/rws2017_moliveira
    catkin_create_pkg player_moliveira std_msgs rospy roscpp

package name should be player_flastname, if you are called **f**irstname **lastname**. _std_msgs rospy roscpp_ are other ros packages from which our package depends. Other may be added later, but its better to always put these from the start.

# Sync your git repo

Now that we have created a new ros package inside the repo, we should synchronize our local copy of the repo with the master version.

    cd ~/catkin_ws/src/rws2017/rws2017_moliveira
    git status
    git add player_moliveira
    git status
    git commit -m "Added a ros package"
    git push

    