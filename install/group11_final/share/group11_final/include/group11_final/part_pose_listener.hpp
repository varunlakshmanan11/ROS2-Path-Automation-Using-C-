/**
 * @file part_pose_listener.hpp
 * @author Nitish Ravisanakar Raveendran - rrnitish@umd.edu,Varun Lakshmanan - varunl11@umd.edu,Sai Jagadeesh Muralikrishnan - jagkrish@umd.edu
 * @brief This file declares the PartPoseListener class. This class is used to listen to the part poses from the logical cameras and aruco markers and send navigation goals to the robot.
 * @version 0.1
 * @date 2023-12-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include <rclcpp/rclcpp.hpp>
#include <mage_msgs/msg/advanced_logical_camera_image.hpp>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include "ros2_aruco_interfaces/msg/aruco_markers.hpp"
#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <string>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>
#include <nav2_msgs/action/navigate_through_poses.hpp>

/**
 * @brief  This class is used to listen to the part poses from the logical cameras and aruco markers and send navigation goals to the robot.
 *
 */
class PartPoseListener : public rclcpp::Node
{
public:
    // PartPoseListener();

    /**
     * @brief  struct to store the waypoints
     *
     */
    struct waypoint
    {
        std::string type;
        std::string color;
        geometry_msgs::msg::Pose pose;
        bool pose_assigned = false;
    };
    std::vector<waypoint> waypoints_;

    /**
     * @brief  struct to store the detected parts
     *
     */
    struct detected_part
    {
        std::string type;
        std::string color;
        geometry_msgs::msg::Pose pose;
    };
    std::vector<detected_part> detected_parts_;
    geometry_msgs::msg::Pose initial_pose_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscription_;

    /**
     * @brief Construct a new part pose listener::part pose listener object
     *
     */
    PartPoseListener() : Node("PartPoseListener"),
                         tf_buffer(this->get_clock()),
                         tf_listener(tf_buffer),
                         aruco_marker_id_(-1),
                         id_received_(false),
                         info_logged_(false),
                         total_parts_to_detect(5),
                         parts_detected(0),
                         all_parts_logged_(false),
                         initial_pose_set_(false),
                         declared(false),
                         current_waypoint_index_(0)
    {

        auto qos = rclcpp::SensorDataQoS();

        // Initialization of the subscribers, publishers and clients
        camera1_subscription = this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>(
            "/mage/camera1/image", qos,
            [this](const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg)
            {
                this->camera_callback(msg, "Camera 1");
            });

        camera2_subscription = this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>(
            "/mage/camera2/image", qos,
            [this](const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg)
            {
                this->camera_callback(msg, "Camera 2");
            });

        camera3_subscription = this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>(
            "/mage/camera3/image", qos,
            [this](const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg)
            {
                this->camera_callback(msg, "Camera 3");
            });
        camera4_subscription = this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>(
            "/mage/camera4/image", qos,
            [this](const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg)
            {
                this->camera_callback(msg, "Camera 4");
            });

        camera5_subscription = this->create_subscription<mage_msgs::msg::AdvancedLogicalCameraImage>(
            "/mage/camera5/image", qos,
            [this](const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg)
            {
                this->camera_callback(msg, "Camera 5");
            });

        aruco_marker_subscription_ = this->create_subscription<ros2_aruco_interfaces::msg::ArucoMarkers>(
            "aruco_markers", 10,
            std::bind(&PartPoseListener::aruco_marker_callback, this, std::placeholders::_1));

        odom_subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10,
            [this](const nav_msgs::msg::Odometry::SharedPtr msg)
            {
                this->odom_callback(msg);
            });
        initialpose_publisher_ = this->create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>("/initialpose", 10);
        navigate_to_pose_client_ = rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, "navigate_to_pose");
        navigation_timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&PartPoseListener::navigate_to_waypoints, this));
    }

private:
    /**
     * @brief  struct to store the part key
     *
     */
    struct part_key
    {
        std::string color;
        std::string type;

        bool operator==(const part_key &other) const
        {
            return color == other.color && type == other.type;
        }
    };

    /**
     * @brief  struct to store the hash of the part key
     *
     */
    struct part_key_hash
    {
        std::size_t operator()(const part_key &key) const
        {
            return std::hash<std::string>()(key.color) ^ std::hash<std::string>()(key.type);
        }
    };

    // Decleration of the variables
    std::unordered_map<part_key, geometry_msgs::msg::Pose, part_key_hash> part_poses_;
    tf2_ros::Buffer tf_buffer;
    tf2_ros::TransformListener tf_listener;
    long aruco_marker_id_;
    bool id_received_;
    bool info_logged_;
    size_t total_parts_to_detect;
    size_t parts_detected;
    bool all_parts_logged_;
    bool initial_pose_set_;
    bool declared;
    size_t current_waypoint_index_;

    /**
     * @brief  Callback function for the logical camera messages. This function receives the logical camera messages and stores the part poses in detected_parts_ vector.
     *
     * @param msg
     * @param camera_name
     */
    void camera_callback(const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg, const std::string &camera_name);

    /**
     * @brief  Callback function for the aruco marker messages. This function receives the aruco marker id and gets the respective parameters and stores it in waypoints_ vector.
     * This fuction also resets the aruco_marker_subscription_ after getting the marker id.
     *
     * @param msg
     */
    void aruco_marker_callback(const ros2_aruco_interfaces::msg::ArucoMarkers::SharedPtr msg);

    /**
     * @brief Checks the map of parts and colors and saves the part type and color to the respective variables.
     *
     * @param part_color
     * @param part_type
     * @param pat_color
     * @param pat_type
     */
    void part_data(int part_color, int part_type, std::string &pat_color, std::string &pat_type);

    /**
     * @brief This function logs the part poses in the terminal.
     *
     */
    void log_all_part_poses();

    /**
     * @brief This function compares the detected parts with the waypoints and updates the pose of the waypoints.
     *
     */
    void process_detected_parts();

    /**
     * @brief This function logs the waypoints in the terminal.
     *
     */
    void log_waypoints();

    /**
     * @brief This fuction is used to get the pose of the robot and publish it to the initialpose topic.
     *
     * @param msg
     */
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg);

    /**
     * @brief This function is used to send the navigation goal to the robot.
     *
     * @param pose
     */
    void send_navigation_goal(const geometry_msgs::msg::Pose &pose);

    /**
     * @brief This function is used to extract each waypoint from the waypoints_ vector and send it to the send_navigation_goal function.
     *
     */
    void navigate_to_waypoints();

    /**
     * @brief This fuction is used to verify if the robot has reached the goal.
     *
     * @param result
     */
    void result_callback(const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::WrappedResult &result);

    // Decleration of the subscribers, publishers and clients
    rclcpp::TimerBase::SharedPtr navigation_timer_;
    rclcpp::Subscription<ros2_aruco_interfaces::msg::ArucoMarkers>::SharedPtr aruco_marker_subscription_;
    rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr initialpose_publisher_;
    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr camera1_subscription;
    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr camera2_subscription;
    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr camera3_subscription;
    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr camera4_subscription;
    rclcpp::Subscription<mage_msgs::msg::AdvancedLogicalCameraImage>::SharedPtr camera5_subscription;
    rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr navigate_to_pose_client_;
    rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr current_goal_handle_;
};
