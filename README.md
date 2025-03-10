# AVCAR-Autonomous-Voice-Controlled-Arm-Robot
Developed AVCAR, an autonomous voice-controlled robotic arm capable of processing natural language voice commands. Designed to assist bedridden and elderly individuals with limited mobility executing tasks such as picking, placing objects. Powered by Robot Operating System 2 (ROS2), Plansys2 , Moveit2, Gazebo, and RViz.

REQUIREMENTS :
    1. Download the package :
    https://github.com/ros-controls/gazebo_ros2_control/blob/master/doc/index.rst
    2. Download the package :
    https://github.com/PlanSys2/ros2_planning_system.git


HOW TO RUN :

    open 5 terminals in this sequence

    ros2 launch plansys2_bt_example plansys2_bt_example_launch.py

    ros2 launch panda_ros2_moveit2 panda_interface.launch.py 

    ros2 run ros2_clients actor 

    ros2 run plansys2_bt_example server 

    ros2 run plansys2_bt_example goalserver

    ros2 run py_pubsub talker




COMMANDS FOR THE VOICE INPUT :
    Objects present ( syrup and can)
    Table divided into 9 grids i.e 9 positions in each grid (position 1 to position 9).

    Voice command examples :
    move can to postion 6
    move syrup to position 9


#   A V C A R 
 
 