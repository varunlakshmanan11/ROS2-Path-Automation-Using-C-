# group11_final ROS2 Package
## Overview
The `group11_final` package is designed for use in ROS2 environments to facilitate interaction with logical cameras and ArUco markers for robotic navigation and object interaction tasks. It listens for part poses from logical cameras and ArUco markers, sending navigation goals to the robot based on the detected poses. This package integrates with the ROS2 navigation stack, specifically designed to work with the `nav2_msgs` action server for navigating to specified poses.

## Features
- Subscribes to logical camera topics to receive images and detect part poses.
- Listens to ArUco marker detections and extracts relevant navigation waypoints.
- Sends navigation goals to the robot to move towards detected parts or predefined waypoints.
- Utilizes the TF2 library for transforming poses between different coordinate frames.
- Compatible with the Nav2 (Navigation2) stack for ROS2.

## Dependencies
- ROS2 Foxy or later
- TF2 ROS
- `nav_msgs`
- `geometry_msgs`
- `mage_msgs` (Custom message package for logical camera images)
- `ros2_aruco_interfaces` (Custom message package for ArUco marker detection)

## Installation
To use the `group11_final` package, clone this repository and move all the contents of the repository to ros2_ws directory.

```bash
git clone https://github.com/Nitish05/ROS2-Path-Automation.git

```
After cloning the repository, you can compile your ROS2 workspace:

```
cd ~/ros2_ws  # Adjust path to your ROS2 workspace
colcon build --symlink-install
source install/setup.bash
```
## Usage
### Launching the Node with `launch.py`
The `group11_final` package includes a launch file named `launch.py` for starting the `part_pose_listener` node with specified parameters. To launch this node, execute:

```
ros2 launch group11_final launch.py
```
This command activates the 'part_pose_listener' node, initiating the process of listening for part poses from logical cameras and ArUco markers, and guiding the robot based on these inputs. Configuration parameters are set within the 'waypoint_params.yaml' file, located in the package's 'config' directory.

## Configuration
Adjust the topic subscriptions within the source code or through ROS2 parameters to align with your project's specific environment setup.

Navigation waypoints, based on ArUco marker detections, are configurable via ROS2 parameters within the 'waypoint_params.yaml' file. Edit this file to tailor the navigation waypoints to your application's needs.

## Nodes
### PartPoseListener
Subscribed Topics
- `/mage/camera[1-5]/image` (mage_msgs/msg/AdvancedLogicalCameraImage)
The images from logical cameras for part detection.
- `/aruco_markers` (ros2_aruco_interfaces/msg/ArucoMarkers)
The detected ArUco markers for navigation waypoint extraction.

Published Topics
- `/initialpose` (geometry_msgs/msg/PoseWithCovarianceStamped)
The initial pose of the robot, published once upon the first odometry message received.
Actions
- `/navigate_to_pose` (nav2_msgs/action/NavigateToPose)
The action client used to send navigation goals to the Nav2 action server.

## License
This package is licensed under the Apache License, Version 2.0 (the "License"); you may not use this package except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.

## Contributors
- Nitish Ravisanakar Raveendran - rrnitish@umd.edu
- Varun Lakshmanan - varunl11@umd.edu
- Sai Jagadeesh Muralikrishnan - jagkrish@umd.edu
