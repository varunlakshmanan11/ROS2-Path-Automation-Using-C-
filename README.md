# PartPoseListener ROS2 Package
## Overview
The PartPoseListener package is designed for use in ROS2 environments to facilitate the interaction with logical cameras and ArUco markers for robotic navigation and object interaction tasks. It listens for part poses from logical cameras and ArUco markers, sending navigation goals to the robot based on the detected poses. This package integrates with the ROS2 navigation stack, specifically designed to work with the nav2_msgs action server for navigating to specified poses.

## Features
Subscribes to logical camera topics to receive images and detect part poses.
Listens to ArUco marker detections and extracts relevant navigation waypoints.
Sends navigation goals to the robot to move towards detected parts or predefined waypoints.
Utilizes the TF2 library for transforming poses between different coordinate frames.
Compatible with the Nav2 (Navigation2) stack for ROS2.

## Dependencies
ROS2 Foxy or later
TF2 ROS
nav_msgs
geometry_msgs
mage_msgs (Custom message package for logical camera images)
ros2_aruco_interfaces (Custom message package for ArUco marker detection)

## Installation
To use the PartPoseListener package, clone this repository into your ROS2 workspace's src directory:

bash
Copy code
cd ~/ros2_ws/src  # Adjust path to your ROS2 workspace
git clone https://github.com/yourusername/PartPoseListener.git
After cloning the repository, you can compile your ROS2 workspace:

bash
Copy code
cd ~/ros2_ws  # Adjust path to your ROS2 workspace
colcon build --symlink-install
source install/setup.bash
Usage
To launch the PartPoseListener node, ensure your ROS2 workspace is sourced correctly, then execute:

bash
Copy code
ros2 run part_pose_listener part_pose_listener
This command starts the PartPoseListener node, which begins listening for part poses from logical cameras and ArUco markers, and navigating the robot accordingly.

Configuration
The PartPoseListener node subscribes to several topics for logical camera images and ArUco markers. You may need to adjust the topic names within the source code to match those used in your environment.

Additionally, the waypoints for navigation based on ArUco marker detections are configurable via ROS2 parameters. These parameters can be set in a parameter file or dynamically during runtime.

Nodes
PartPoseListener
Subscribed Topics
/mage/camera[1-5]/image (mage_msgs/msg/AdvancedLogicalCameraImage)
The images from logical cameras for part detection.
/aruco_markers (ros2_aruco_interfaces/msg/ArucoMarkers)
The detected ArUco markers for navigation waypoint extraction.
Published Topics
/initialpose (geometry_msgs/msg/PoseWithCovarianceStamped)
The initial pose of the robot, published once upon the first odometry message received.
Actions
/navigate_to_pose (nav2_msgs/action/NavigateToPose)
The action client used to send navigation goals to the Nav2 action server.
License
This package is provided under [specify your license], which allows for modification, distribution, and use within the specified terms.

Contributors
Nitish Ravisanakar Raveendran - rrnitish@umd.edu
Varun Lakshmanan - varunl11@umd.edu
Sai Jagadeesh Muralikrishnan - jagkrish@umd.edu
This README provides a basic overview for getting started with the PartPoseListener package. For more detailed information, please refer to the specific documentation within the code.
