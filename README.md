# diffdrive_arduino
Thanks Josh Newans for this awesome hardware interface
This node is designed to provide a ros2_control hardware interface for an Arduino running firmware from `ros_arduino_bridge`.
It is designed to be used with a `diff_drive_controller` from `ros2_control`.
It is expected to communicate via serial and to have two motors, each with velocity control and position/velocity feedback.




It is based on the diffbot example from [ros2_control demos](https://github.com/ros-controls/ros2_control_demos/tree/master/example_2).

For a tutorial on how to develop a hardware interface like this, check out the video below:

https://youtu.be/J02jEKawE5U



## To Do

- [ ] It should be hosted on Raspberry Pi and remove the need for the socket communications
