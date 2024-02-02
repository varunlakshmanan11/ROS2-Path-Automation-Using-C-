#include "part_pose_listener.hpp"

void PartPoseListener::part_data(int part_color, int part_type, std::string &pat_color, std::string &pat_type)
{
    switch (part_color)
    {
    case mage_msgs::msg::Part::BLUE:
        pat_color = "BLUE";
        break;
    case mage_msgs::msg::Part::GREEN:
        pat_color = "GREEN";
        break;
    case mage_msgs::msg::Part::ORANGE:
        pat_color = "ORANGE";
        break;
    case mage_msgs::msg::Part::RED:
        pat_color = "RED";
        break;
    case mage_msgs::msg::Part::PURPLE:
        pat_color = "PURPLE";
        break;
    }

    switch (part_type)
    {
    case mage_msgs::msg::Part::BATTERY:
        pat_type = "BATTERY";
        break;
    case mage_msgs::msg::Part::PUMP:
        pat_type = "PUMP";
        break;
    case mage_msgs::msg::Part::SENSOR:
        pat_type = "SENSOR";
        break;
    case mage_msgs::msg::Part::REGULATOR:
        pat_type = "REGULATOR";
        break;
    }
    std::transform(pat_color.begin(), pat_color.end(), pat_color.begin(), ::toupper);
    std::transform(pat_type.begin(), pat_type.end(), pat_type.begin(), ::toupper);
}

void PartPoseListener::camera_callback(const mage_msgs::msg::AdvancedLogicalCameraImage::SharedPtr msg, const std::string &camera_name)
{
    std::string camera_frame;

    try
    {
        if (camera_name == "Camera 1")
        {
            camera_frame = "camera1_frame";
        }
        else if (camera_name == "Camera 2")
        {
            camera_frame = "camera2_frame";
        }
        else if (camera_name == "Camera 3")
        {
            camera_frame = "camera3_frame";
        }
        else if (camera_name == "Camera 4")
        {
            camera_frame = "camera4_frame";
        }
        else if (camera_name == "Camera 5")
        {
            camera_frame = "camera5_frame";
        }
        else
        {
            RCLCPP_ERROR(this->get_logger(), "Unknown camera: %s", camera_name.c_str());
            return;
        }

        if (!info_logged_)
        {
            for (const auto &part_pose : msg->part_poses)
            {
                std::string pat_color, pat_type;
                part_data(part_pose.part.color, part_pose.part.type, pat_color, pat_type);

                geometry_msgs::msg::PoseStamped stamped_pose;
                stamped_pose.header.frame_id = camera_frame;
                stamped_pose.header.stamp = this->get_clock()->now();
                stamped_pose.pose = part_pose.pose;

                geometry_msgs::msg::TransformStamped transformStamped = tf_buffer.lookupTransform(
                    "map", camera_frame, tf2::TimePointZero);
                geometry_msgs::msg::PoseStamped pose_transformed;
                tf2::doTransform(stamped_pose, pose_transformed, transformStamped);

                part_key key{pat_color, pat_type};

                if (part_poses_.find(key) == part_poses_.end())
                {
                    part_poses_[key] = pose_transformed.pose;
                    parts_detected++;
                    detected_part detected_part{pat_type, pat_color, pose_transformed.pose};
                    detected_parts_.push_back(detected_part);

                    if (parts_detected >= total_parts_to_detect && !all_parts_logged_)
                    {
                        log_all_part_poses();
                        all_parts_logged_ = true;
                        camera1_subscription.reset();
                        camera2_subscription.reset();
                        camera3_subscription.reset();
                        camera4_subscription.reset();
                        camera5_subscription.reset();
                        info_logged_ = true;
                        break;
                    }
                }
            }
            process_detected_parts();
            navigate_to_waypoints();
        }
    }
    catch (tf2::TransformException &ex)
    {
        RCLCPP_WARN(this->get_logger(), "Failed to transform pose from %s to world frame: %s", camera_frame.c_str(), ex.what());
    }
}

void PartPoseListener::aruco_marker_callback(const ros2_aruco_interfaces::msg::ArucoMarkers::SharedPtr msg)
{
    if (!msg->marker_ids.empty())
    {
        long aruco_id = msg->marker_ids[0];
        aruco_marker_subscription_.reset();
        if (!declared)
        {
            for (int i = 0; i < 5; ++i)
            {
                std::string base_param_path = "aruco_" + std::to_string(aruco_id) + ".wp" + std::to_string(i + 1);
                this->declare_parameter<std::string>(base_param_path + ".type", "default_type");
                this->declare_parameter<std::string>(base_param_path + ".color", "default_color");
            }
            declared = true;
        }
        std::string base_param_path = "aruco_" + std::to_string(aruco_id);

        for (int i = 0; i < 5; ++i)
        {
            std::string wp_key = "wp" + std::to_string(i + 1);
            std::string type_key = base_param_path + "." + wp_key + ".type";
            std::string color_key = base_param_path + "." + wp_key + ".color";

            waypoint waypoint;
            this->get_parameter(type_key, waypoint.type);
            this->get_parameter(color_key, waypoint.color);
            waypoints_.push_back(waypoint);
        }
    }
}

void PartPoseListener::log_all_part_poses()
{
    for (const auto &entry : part_poses_)
    {
        const auto &key = entry.first;
        const auto &pose = entry.second;
        RCLCPP_INFO(this->get_logger(), "Part: Color = %s, Type = %s, Pose: x = %f, y = %f, z = %f",
                    key.color.c_str(), key.type.c_str(), pose.position.x, pose.position.y, pose.position.z);
    }
}

void PartPoseListener::process_detected_parts()
{
    for (const auto &detected_part : detected_parts_)
    {
        RCLCPP_INFO(this->get_logger(), "Processing Detected Part: Type = %s, Color = %s, Pose: [x = %f, y = %f, z = %f]",
                    detected_part.type.c_str(), detected_part.color.c_str(),
                    detected_part.pose.position.x, detected_part.pose.position.y, detected_part.pose.position.z);
        std::string detected_type = detected_part.type;
        std::string detected_color = detected_part.color;

        std::transform(detected_type.begin(), detected_type.end(), detected_type.begin(), ::toupper);
        std::transform(detected_color.begin(), detected_color.end(), detected_color.begin(), ::toupper);

        for (auto &waypoint : waypoints_)
        {
            std::string waypoint_type = waypoint.type;
            std::string waypoint_color = waypoint.color;

            std::transform(waypoint_type.begin(), waypoint_type.end(), waypoint_type.begin(), ::toupper);
            std::transform(waypoint_color.begin(), waypoint_color.end(), waypoint_color.begin(), ::toupper);

            if (waypoint_type == detected_type && waypoint_color == detected_color && !waypoint.pose_assigned)
            {
                waypoint.pose = detected_part.pose;
                waypoint.pose.position.z = 0.0;
                waypoint.pose_assigned = true;
                break;
            }
        }
    }
    log_waypoints();
}

void PartPoseListener::log_waypoints()
{
    for (const auto &waypoint : waypoints_)
    {
        RCLCPP_INFO(this->get_logger(), "waypoint: Type = %s, Color = %s, Pose: [x = %f, y = %f, z = %f]",
                    waypoint.type.c_str(), waypoint.color.c_str(),
                    waypoint.pose.position.x, waypoint.pose.position.y, waypoint.pose.position.z);
    }
}

void PartPoseListener::odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg)
{
    if (!initial_pose_set_)
    {
        initial_pose_ = msg->pose.pose;
        initial_pose_.position.z = 0.0;
        initial_pose_set_ = true;
        RCLCPP_INFO(this->get_logger(), "Initial pose set: [x = %f, y = %f, z = %f]",
                    initial_pose_.position.x, initial_pose_.position.y, initial_pose_.position.z);

        geometry_msgs::msg::PoseWithCovarianceStamped pose_msg;
        pose_msg.header.stamp = this->get_clock()->now();
        pose_msg.header.frame_id = "map";
        pose_msg.pose.pose = initial_pose_;

        initialpose_publisher_->publish(pose_msg);
    }
}

void PartPoseListener::send_navigation_goal(const geometry_msgs::msg::Pose &pose)
{
    if (!navigate_to_pose_client_->wait_for_action_server())
    {
        RCLCPP_ERROR(this->get_logger(), "Action server not available");
        return;
    }

    auto goal_msg = nav2_msgs::action::NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "map";
    goal_msg.pose.pose = pose;

    auto send_goal_options = rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SendGoalOptions();
    send_goal_options.goal_response_callback =
        [this](const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr &goal_handle)
    {
        if (!goal_handle)
        {
            // RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        }
        else
        {
            this->current_goal_handle_ = goal_handle;
        }
    };

    send_goal_options.result_callback = std::bind(&PartPoseListener::result_callback, this, std::placeholders::_1);

    navigate_to_pose_client_->async_send_goal(goal_msg, send_goal_options);
}

void PartPoseListener::navigate_to_waypoints()
{
    if (waypoints_.empty())
    {
        RCLCPP_WARN(this->get_logger(), "No waypoints to navigate to.");
        return;
    }

    if (current_waypoint_index_ < 5)
    {
        const auto &waypoint = waypoints_[current_waypoint_index_];
        if (waypoint.pose_assigned)
        {
            send_navigation_goal(waypoint.pose);
        }
    }
}

void PartPoseListener::result_callback(
    const rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::WrappedResult &result)
{
    switch (result.code)
    {
    case rclcpp_action::ResultCode::SUCCEEDED:
        RCLCPP_INFO(this->get_logger(), "Reached waypoint %zu successfully", current_waypoint_index_);
        current_waypoint_index_++;
        if (current_waypoint_index_ < waypoints_.size() && current_waypoint_index_ < 5)
        {
            const auto &next_waypoint = waypoints_[current_waypoint_index_];
            if (next_waypoint.pose_assigned)
            {
                send_navigation_goal(next_waypoint.pose);
            }
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Reached the 5th waypoint or all waypoints have been reached");
        }
        break;
    case rclcpp_action::ResultCode::ABORTED:

        break;
    case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_INFO(this->get_logger(), "Goal was canceled");
        break;
    default:
        RCLCPP_ERROR(this->get_logger(), "Unknown result code");
        break;
    }
}

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PartPoseListener>());
    rclcpp::shutdown();
    return 0;
}