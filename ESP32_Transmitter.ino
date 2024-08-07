#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <esp_now.h>
#include <WiFi.h>
#include <time.h>
#include <Arduino.h>
//#include "ESP32Time.h"

//Huzzah32
#define led LED_BUILTIN
#define BD 115200
const int AN_Out =A2;
const int SEL_0 =A12;
const int SEL_1 =A11;
const int SEL_2 =A10;
int AN_Val;
int CH_Val[7];

//SD Card
const int SD_CS =4;
String dataMessage;
String timeMessage;
String tempMSG;

//ESP_NOW
//uint8_t broadcastAddress[] = {0xC4, 0xDD, 0x57, 0x9E, 0xA8, 0x18};  // REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x94, 0xb9, 0x7e, 0x6b, 0x68, 0xd8};  // REPLACE WITH YOUR RECEIVER MAC Address
typedef struct struct_message {
  unsigned long timeStamp;
  int CH_Value[7];
} struct_message;

struct_message myData;

//RTC
//const char* ntpServer="pool.ntp.org";
//const long gmtOffset_sec=8*3600;
//const int dalightOffset_sec=0;

void SD_setup(){
  Serial.println(MOSI);
  Serial.println(MISO);
  Serial.println(SCK);
  Serial.println(SS);

  Serial.begin(BD);
  //Serial.begin(460800);
  SD.begin(4);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  File file = SD.open("/log.txt");
  if (!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/log.txt", "Time , CH1_RAW , CH2_RAW, CH3_RAW, CH4_RAW \r\n");
  }
  else {
    Serial.println("File already exists");
    deleteFile(SD, "/log.txt");
    delay(400);
    writeFile(SD, "/log.txt", "Time , CH1_RAW , CH2_RAW, CH3_RAW, CH4_RAW \r\n");
  }
  file.close();
}

void ESP_Now_setup(){
  WiFi.mode(WIFI_STA);
   // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_peer_info_t peerInfo;
  // Register peer
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
 
  //esp_now_register_send_cb(OnDataSent);
  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
   // Add peer        
  if (addStatus == ESP_OK) {
    // Pair success
    Serial.println("Pair success");
  } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW Not Init");
  } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
    Serial.println("Peer list full");
  } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("Out of memory");
  } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
    Serial.println("Peer Exists");
  } else {
    Serial.println("Not sure what happened");
  }
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

int64_t xx_time_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo,"%A,%B %d %Y %H:%M:%S");
}

int64_t gettime(){
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
}

int64_t getTime_Test(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t time_us = tv.tv_usec;
}

void logSDCard() {
  unsigned long currentTime=millis();
  myData.timeStamp=currentTime;
  dataMessage = String(currentTime) + "," +
                String(myData.CH_Value[0]) + ", " + String(myData.CH_Value[1]) + ", " +
                String(myData.CH_Value[2]) + ", " + String(myData.CH_Value[3]) + ", " +
                String(myData.CH_Value[4]) + ", " + String(myData.CH_Value[5]) + ", " +
                String(myData.CH_Value[6]) + ", " + String(myData.CH_Value[7]) +"\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/log.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void setup() {
  Serial.begin(BD);
  //Serial.begin(460800);
  while (!Serial) {    // 等待串列埠連線
  }

  ESP_Now_setup();
  SD_setup();

  // set LED to be an output pin
  pinMode(led, OUTPUT);
  pinMode(AN_Out, INPUT);
  pinMode(SEL_0,OUTPUT);
  pinMode(SEL_1,OUTPUT);
  pinMode(SEL_2,OUTPUT);
  init();
}

void init(){
  Serial.print("\nWaiting for SD card ready...");
  digitalWrite(led, HIGH);
  digitalWrite(SEL_0, LOW); 
  digitalWrite(SEL_1, LOW); 
  digitalWrite(SEL_2, LOW);
  delay(5000);                // wait for a half second

}

void SetCH0(){
  //SEL0=0 SEL1=0  Case0
  digitalWrite(SEL_0, LOW); 
  digitalWrite(SEL_1, LOW);  
  digitalWrite(SEL_2, LOW);
  delay(1);
         
}

void SetCH1(){
 //SEL0=1 SEL1=0 Case1
  digitalWrite(SEL_0, HIGH); 
  digitalWrite(SEL_1, LOW);
  digitalWrite(SEL_2, LOW);  
  delay(1);

}

void SetCH2(){
  //SEL0=0 SEL1=1 Case2
  digitalWrite(SEL_0, LOW); 
  digitalWrite(SEL_1, HIGH);
  digitalWrite(SEL_2, LOW);
  delay(1);

}

void SetCH3(){
  //SEL0=1 SEL1=1 Case3
  digitalWrite(SEL_0, HIGH); 
  digitalWrite(SEL_1, HIGH);
  digitalWrite(SEL_2, LOW);
  delay(1);

}

void SetCH4(){
  //SEL0=0 SEL1=0  Case0
  digitalWrite(SEL_0, LOW); 
  digitalWrite(SEL_1, LOW);  
  digitalWrite(SEL_2, HIGH);
  delay(1);
         
}

void SetCH5(){
  //SEL0=0 SEL1=0  Case0
  digitalWrite(SEL_0, HIGH); 
  digitalWrite(SEL_1, LOW);  
  digitalWrite(SEL_2, HIGH);
  delay(1);
         
}

void SetCH6(){
  //SEL0=0 SEL1=0  Case0
  digitalWrite(SEL_0, LOW); 
  digitalWrite(SEL_1, HIGH);  
  digitalWrite(SEL_2, HIGH);
  delay(1);
         
}

void SetCH7(){
  //SEL0=0 SEL1=0  Case0
  digitalWrite(SEL_0, HIGH); 
  digitalWrite(SEL_1, HIGH);  
  digitalWrite(SEL_2, HIGH);
  delay(1);
         
}

void loop() {
  SetCH0();
  AN_Val =analogRead(AN_Out);
  //CH_Val[0]=AN_Val;
  myData.CH_Value[0]=AN_Val;
  
  SetCH1();
  AN_Val =analogRead(AN_Out);
  //CH_Val[1]=AN_Val;
  myData.CH_Value[1]=AN_Val;
   
  SetCH2();
  AN_Val =analogRead(AN_Out);
  //CH_Val[2]=AN_Val;
  myData.CH_Value[2]=AN_Val;
  
  SetCH3();
  AN_Val =analogRead(AN_Out);
  //CH_Val[3]=AN_Val;
  myData.CH_Value[3]=AN_Val;
  
  SetCH4();
  AN_Val =analogRead(AN_Out);
  //CH_Val[3]=AN_Val;
  myData.CH_Value[4]=AN_Val;
  
  SetCH5();
  AN_Val =analogRead(AN_Out);
  //CH_Val[3]=AN_Val;
  myData.CH_Value[5]=AN_Val;
  
  SetCH6();
  AN_Val =analogRead(AN_Out);
  //CH_Val[3]=AN_Val;
  myData.CH_Value[6]=AN_Val;
  
  SetCH7();
  AN_Val =analogRead(AN_Out);
  //CH_Val[3]=AN_Val;
  myData.CH_Value[7]=AN_Val;
  /*
  for(int i=0; i<4; i++){ 
  Serial.print("CH_VAL[");
  Serial.print(i);
  Serial.print("]=");
  Serial.println(CH_Val[i]);
  }
  */
  //printLocalTime();
  logSDCard();
  //Serial.println(AN_Val);   

  
  //strcpy(myData.a, tempMSG.c_str());
  
  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  /*
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else{
    Serial.println("Error sending the data");
  }
  */

  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  delay(1);                // wait for a half second

}
 