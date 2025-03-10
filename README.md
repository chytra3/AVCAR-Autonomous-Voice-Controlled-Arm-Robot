# AVCAR - Autonomous Voice-Controlled Arm Robot

AVCAR is an autonomous, voice-controlled robotic arm that processes natural language voice commands. It is designed to assist bedridden and elderly individuals with limited mobility in performing tasks such as picking and placing objects.  

Powered by **ROS2(HUMBLE), Plansys2, Moveit2, Gazebo, and RViz**.

---

## 📌 Requirements

1. Download the package: [Gazebo_Ros2_Control](https://github.com/ros-controls/gazebo_ros2_control/blob/master/doc/index.rst)
2. Download the package: [Ros2_Planning_System](https://github.com/PlanSys2/ros2_planning_system.git)

---

## 🚀 How to Run 

Open **5 terminals** in the following sequence:

```bash
ros2 launch plansys2_bt_example plansys2_bt_example_launch.py
ros2 launch panda_ros2_moveit2 panda_interface.launch.py
ros2 run ros2_clients actor
ros2 run plansys2_bt_example server
ros2 run plansys2_bt_example goalserver
ros2 run py_pubsub talker
```

---


## 🎧 Commands for Voice Input  
- **Objects Available**: `syrup`, `can`  
- **Table Layout**: Divided into **9 grid positions** (position 1 to position 9)  

### **Example Commands**
```text
move can to position 6
move syrup to position 9
```

---

## 🖥️ Demo  
### System Architecture  
![System Architecture](https://github.com/chytra3/AVCAR-Autonomous-Voice-Controlled-Arm-Robot/blob/ecbf2c3e2d0a15e2c81e473746e979c13d3e1741/system_architecture.jpg)  
### Tutorial  
[🎥 Watch the Tutorial](https://github.com/user-attachments/assets/e25855a5-3065-4b58-9640-da1dd22175ec)

---
