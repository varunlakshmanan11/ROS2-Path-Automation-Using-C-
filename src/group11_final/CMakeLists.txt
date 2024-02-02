cmake_minimum_required(VERSION 3.8)
project(group11_final)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# include folder which has all the header files
include_directories(include/group11_final)

# Create a variable for all dependencies
set(FRAME_DEMO_INCLUDE_DEPENDS
  rclcpp 
  tf2_ros
  geometry_msgs
  tf2
  tf2_geometry_msgs
  std_msgs
  ros2_aruco_interfaces
  mage_msgs
  nav_msgs
  rosgraph_msgs
  nav2_msgs
  rclcpp_action
)

# Find all dependencies
foreach(dependency IN ITEMS ${FRAME_DEMO_INCLUDE_DEPENDS})
  find_package(${dependency} REQUIRED)
endforeach()

#-----------------------------
# C++
#-----------------------------

add_executable(part_pose_listener src/part_pose_listener.cpp)
ament_target_dependencies(part_pose_listener ${FRAME_DEMO_INCLUDE_DEPENDS})

# Install the executable
install(TARGETS part_pose_listener
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# Install directories
install(DIRECTORY include config launch DESTINATION share/${PROJECT_NAME}/)

# Finalize ament package
ament_package()
