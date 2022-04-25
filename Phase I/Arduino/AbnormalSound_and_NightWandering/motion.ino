#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Arduino_MKRENV.h>


// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// Watson IoT connection details
// Xingyu
// ----------------------------------------------
// #define MQTT_HOST "dva68t.messaging.internetofthings.ibmcloud.com"
// #define MQTT_PORT 1883
// #define MQTT_DEVICEID "d:dva68t:mkr1010:4c11ae916fb0"
// #define MQTT_USER "use-token-auth"
// #define MQTT_TOKEN "mkr1010_4c11ae916fb0"
// ----------------------------------------------

// Jinyang
#define MQTT_HOST "ts9xp8.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:ts9xp8:mkr1010:7c9ebd3b691c"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "mkr1010_7c9ebd3b691c"
// ----------------------------------------------

#define MQTT_TOPIC "iot-2/evt/status/fmt/json"

#define MQTT_TOPIC_alarm "iot-2/cmd/alarm/fmt/json"


// Add WiFi connection information
//char ssid[] = "ABCD";     //  your network SSID (name) ABC
//char pass[] = "123456789";  // your network password 123456789

char ssid[] = "random";     //  your network SSID (name) ABC
char pass[] = "135798642";  // your network password 123456789

int WiFistatus = WL_IDLE_STATUS;     // the Wifi radio's status
const int motionSensor = 0;
int motionSensorState;

const int buzzer = 1;
// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

void normal();
void abnormal();
void hospital();
void patient();
void buzzer_ring(int freq, int delay_time);


// variables to hold data
StaticJsonDocument<500> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[500];
// char alert;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");

  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);

  
  // ------------------------------------------------------------
   StaticJsonDocument<256> docRcv;
   deserializeJson(docRcv, payload);
   
  if (0 == strcmp(topic, MQTT_TOPIC_alarm)) {
    //valid message received
    Serial.println("alarm received!!!");
    int alarm_value = docRcv["alarm"];
    if(alarm_value == 1){ // Green Light
      normal();
    }
    else if(alarm_value == 2){ // Yellow light + buzzer
      abnormal();
    } else if (alarm_value == 3){ // Purple light + buzzer
      hospital();
    } else if(alarm_value == 4){ // Patient is calling its roommate for help, Red light + buzzer
      patient();
    }

  }

  else {
    Serial.println("Unknown command received");
  }

}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  pinMode(motionSensor, INPUT);
  pinMode(buzzer, OUTPUT); 
  
  // if (!ENV.begin()) { //check correct initialisation of shield
  //   Serial.println("Failed to initialize MKR ENV shield!");
  //   while (1); //infinite loop, avoids loop to be executed
  // }
  // while (!Serial) {
  //   ; // wait for serial port to connect. Needed for native USB port only
  // }

  // attempt to connect to Wifi network:

  while (WiFistatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    WiFistatus = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  
  Serial.println("----------------------------------------");
  printData();
  Serial.println("----------------------------------------");

  // Connect to MQTT - IBM Watson IoT Platform
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_alarm);

  } else {
    Serial.println("MQTT Failed to connect!");
  }
}

void loop() {
  mqtt.loop();
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_alarm);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  motionSensorState = digitalRead(motionSensor);
  if (motionSensorState == 1){
    status["Motion"] = motionSensorState;

    
    serializeJson(jsonDoc, msg, 500);
    Serial.println(msg);
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }

    for (int i = 0; i < 5; i++) {
      mqtt.loop();
      delay(1000);
    }

  }




 
}

void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

// -----------------------------------------


void normal()
{
  // green led 
  WiFiDrv::analogWrite(25, 255); //green LED switched on
  WiFiDrv::analogWrite(26, 0); 
  WiFiDrv::analogWrite(27, 0);

  // delay(1000); //1s

  // WiFiDrv::analogWrite(25, 0);
  // WiFiDrv::analogWrite(26, 0);
  // WiFiDrv::analogWrite(27, 0);
}

// the patient has some abnormal situations > threshold
// 室友那里灯黄色 + 蜂鸣器 + email
// 医院：email-->看dashboard
void abnormal()
{
  // yellow led
  WiFiDrv::analogWrite(25, 255); //yellow LED switched on
  WiFiDrv::analogWrite(26, 255);
  WiFiDrv::analogWrite(27, 0);
  //buzzer
  buzzer_ring(1000, 2000);

  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);
}

// Hospital seeks roommates to check on patients
// 室友那里灯紫色 + 蜂鸣器 + email + sms
void hospital()
{
  //purple led
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 255); //purple LED switched on
  WiFiDrv::analogWrite(27, 255);
  // buzzer
  buzzer_ring(1500, 4000);

  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);  
}

// the patient asks for help
// 室友那里灯红色 + 蜂鸣器 + email + sms
// 医院：email + sms
void patient()
{
  // red led
  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 255); //red LED switched on
  WiFiDrv::analogWrite(27, 0);

  // buzzer  
  buzzer_ring(2000, 6000);

  // delay(1000); //1s

  WiFiDrv::analogWrite(25, 0);
  WiFiDrv::analogWrite(26, 0);
  WiFiDrv::analogWrite(27, 0);


}

void buzzer_ring(int freq, int delay_time){
  tone(buzzer, freq); // Send 1KHz sound signal...
  delay(delay_time);        // ...for 1 sec
  noTone(buzzer);     // Stop sound...
  // delay(delay_time);        // ...for 1sec


}
