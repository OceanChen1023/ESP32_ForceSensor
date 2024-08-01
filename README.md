# **Micro-ROS Adafruit ESP32 feather 環境建置**

**Material Request:**
1. Adafruit EPS32 feather
2. Micro-USB to USB cable


**Environment:**
1.Ubuntu 22.04 humble
2.Ros
3.Ros2
4.Micro-Ros
5.Arduino IDE



**ROS & ROS2 Env. Establish**
Before install ROS2, must to install ROS first, following the official tutorial [https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html](https://docs.ros.org/en/humble/Installation/Ubuntu-Install-Debians.html) 


**Arduino IDE Env. Establish**
Step1:Download Arduino IDE from official website [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
Step2:Extract to ./Arduino or any directory you want
Step3:Install ESP32 library(choose "esp32 by Espressif Systems")
Step4:Micro-ROS Library Setting, Following Sketch>>Include Library>>Add zip library>>Choose "micro_ros_arduino-2.0.7-humble" download zip from [https://github.com/micro-ROS/micro_ros_arduino/releases/tag/v2.0.7-humble](https://github.com/micro-ROS/micro_ros_arduino/releases/tag/v2.0.7-humble)

Issue:At beginning, you execute programm maybe show error "No module 'serial'", just keyin command on terminal 
```
pip3 install pyserial
```



**Micro Ros and Arduino IDE establishment, reference this website**
[https://www.hackster.io/514301/micro-ros-on-esp32-using-arduino-ide-1360ca](https://www.hackster.io/514301/micro-ros-on-esp32-using-arduino-ide-1360ca)


Final, When you run ros2 program, you should check your serial comport name, it always is </dev/ttyUSB0> instead of the comport choosen.

```
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0
```

If ROS2 don't dectect your serial comport. The second command should restart OS and it work permanently.
```
sudo chmod 777 /dev/ttyUSB0
sudo usermod -a -G dialout <Your_user_name>
```







