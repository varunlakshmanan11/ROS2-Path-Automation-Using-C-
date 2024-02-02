- After unziping the file and placeing the package in the src folder of your workspace, 
    You can build the package using from the root of the workspace using the command 
    colcon build (or) colcon buid --packages-select group11_final

- To run the package, 
    source the setup.bash file using the command 
    source install/setup.bash

    Then launch the turtlebot in gazebo using the command
    "ros2 launch final_project final_project.launch.py" as given in the project documentation

    Then launch the node using the command
    "ros2 launch group11_final launch.py" 

- Expected Behavior:
    when you launch the node it will lauch the waypoint_params.yaml aswell

    when the node is running you can see:

                     """   There is a delay before the turtlebot starts moving. It will move to each waypoint then
                     a second to calculate the path to the next waypoint   """

                     intial pose that has been passed to the topic /intialpose and log the pose in the terminal.
                     the part type it's color and postion that has been detected by the logical cameras.
                     the waypoints in the correct order from the params file that has been got by using the aruco marker id.
                     if the turtlebot reached a waypoint it will log it in the terminal.


- If nothing works please reachout to use.

This is the link for the youtube video of the package working on our machine. https://youtu.be/N27e0BAGu2Q

We have reffered the navigation_demo.zip for the code sctucture and the naming convention
