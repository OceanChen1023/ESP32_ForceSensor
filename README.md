# **Adafruit ESP32 feather無線壓力感測器傳輸系統**

## 摘要 
使用兩塊ESP32，其中一塊做為8個力感測器資料接收端並透過WIFI發送到另一塊ESP32，ESP32接收端同時透過Micro-ROS將力感測器資料傳送至ROS2系統當中，經測試力感測器資料接收頻率約62Hz，符合設計要求。
![image](https://hackmd.io/_uploads/SJmdfKgqC.png)


## Github檔案內容簡述
ESP32_Transmiiter: ESP32發送端

ESP32_Receiver: ESP32接收端

ESP32_Read_Mac_Address: 因使用MAC Address方式傳輸，需要先讀取接收端MAC Address，後發送端設定

Press_Sensor_Layout: 電路設計圖以及PCB Layout


## 系統環境架設

**Material Request:**
1. Adafruit EPS32 feather
2. Micro-USB to USB cable


**Environment:**

1.Ubuntu 22.04 humble

2.Ros2

3.Micro-Ros

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







