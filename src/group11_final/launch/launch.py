import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('group11_final'),
        'config',
        'waypoint_params.yaml'
    )

    node = Node(
        package='group11_final',
        executable='part_pose_listener',
        name='part_pose_listener',
        output='screen',
        parameters=[config]
    )

    return LaunchDescription([node])