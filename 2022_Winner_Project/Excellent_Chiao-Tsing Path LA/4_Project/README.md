# 2022-Synopsys-ARC-Contest-ChiaoTsingTrailLA
![](https://i.imgur.com/JyPJqi2.gif)

# Introduction
* Our robot can track body temperature, and use deep learning to recognize people who are not wearing a mask. When it discovers a person who is not wearing a mask, it will track the person and launch a mask projectile onto the person’s face.
* With ARC EM9D board’s ability to calculate Neural Network very fast, we can use image recognition on the robot without having to use a remote server to do calculations. With this powerful board, we can create our awesome Mask Supervise Robot!
* We use the wifi chip on Raspberry Pi to control the robot remotely. 

# Demo
https://www.youtube.com/watch?v=bebB13EzUDo&t=9s


# HW/SW Setup
![](https://i.imgur.com/j6L3zfO.png)

## The Robot
The robot made from LEGO and Woods. 
* The Artillery(口罩發射砲台)
    * Controlled by Raspberry Pi with two motors which is made from LEGO.

    * There is a platform for loading the mask which is made of cartbord and pipes. The mask will be shot of by air bottle through the pipes.

* The Continuous Track(履帶)
    * Made from LEGO. Enable the robot traverse different kinds of landform with less likelihood of becoming stuck.

## ARC EM9D
1. Upload our code through ARC MetaWare.
2. Stab the board through USB port to the Raspberry Pi.

## Raspberry Pi
1. Install linux OS.
2. Install ROS Noetic.
3. Copy the controling code written in python to the Pi.
4. Run the code

# User manual
1. loading the mask and air bottle to the robot.
2. Setting up the ARC EM9D and Raspberry Pi.
3. Run the code on the 
4. We can control the robot by the Joystick. The screen on the compute shows the view of camera on the ARC EM9D.
    * Left analog stick: Robot movement
    * Right analog stick: Artillery pitch and yaw
    * Right trigger: Fire the mask 