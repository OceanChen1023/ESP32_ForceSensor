/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
 #include <micro_ros_arduino.h>

#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/u_int32_multi_array.h>
#include <std_msgs/msg/u_int8_multi_array.h>

#include <esp_now.h>
#include <WiFi.h>

#define BD 115200
#define AGENT_SERIAL Serial
 
// Structure example to receive data
// Must match the sender structure
 

rcl_publisher_t publisher;
//rcl_publisher_options_t publisher_options=rcl_publisher_get_default_options();
//publisher_options.qos = rcl_qos_profile_default;

std_msgs__msg__Int32 msg;
std_msgs__msg__UInt32MultiArray uint32_array_msg;
std_msgs__msg__UInt8MultiArray uint8_array_msg;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define LED_PIN 13
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}



typedef struct struct_message {
  uint32_t timeStamp;
  uint32_t CH_Value[4];
} struct_message;

uint32_t data[5];


String ReceiveMessage;
// Create a struct_message called myData
struct_message myData;
 
//Micro-ros Segment
void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(10);
  }
}

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{  
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    RCSOFTCHECK(rcl_publish(&publisher, &uint32_array_msg, NULL));
    //msg.data++;
  }
}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint32_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println(myData.timeStamp);
  Serial.println(myData.CH_Value[0]);
  Serial.println(myData.CH_Value[1]);
  Serial.println(myData.CH_Value[2]);
  Serial.println(myData.CH_Value[3]);


  data[0] = myData.timeStamp;
  data[1] = myData.CH_Value[0];
  data[2] = myData.CH_Value[1];
  data[3] = myData.CH_Value[2];
  data[4] = myData.CH_Value[3];
  uint32_array_msg.data.size = 5;
}
 
void setup() {
  //Micro-Ros setup
  AGENT_SERIAL.begin(BD);
  set_microros_transports();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  delay(2000);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_arduino_node", "", &support));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
   &publisher,
   &node,
   ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt32MultiArray),
   "micro_ros_arduino_node_publisher1"));

  // create timer,
  const unsigned int timer_timeout = 1;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback));

  // create executor
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  //msg.data = 0;
  uint32_array_msg.data.data = data;
  uint32_array_msg.data.size = 5;
  uint32_array_msg.data.capacity = sizeof(data);


  // Initialize Serial Monitor
  //Serial.begin(115200);
  Serial.begin(BD); 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}
 
void loop() {

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  //RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1)));
  RCSOFTCHECK(rcl_publish(&publisher, &uint32_array_msg, NULL));


}