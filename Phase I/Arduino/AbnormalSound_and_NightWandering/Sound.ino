#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#include <utility/wifi_drv.h>
#include <cmath>

// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// Watson IoT connection details
#define MQTT_HOST "ts9xp8.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:ts9xp8:mkr1010:7c9ebd3b691c"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "mkr1010_7c9ebd3b691c"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_threshold "iot-2/cmd/threshold/fmt/json"
#define MQTT_TOPIC_alarm "iot-2/cmd/alarm/fmt/json"
#define MQTT_TOPIC_INTERVAL "iot-2/cmd/INTERVAL/fmt/json"

// Add WiFi connection information
//char ssid[] = "ABCD";     //  your network SSID (name)
//char pass[] = "123456789";  // your network password
//
char ssid[] = "random";    //  your network SSID (name)
char pass[] = "135798642";  // your network password

int WiFistatus = WL_IDLE_STATUS;     // the Wifi radio's status

// --------------------------------------------------------------------------------------------
//         Initially define the parameters which will be updated by the dashboard
// --------------------------------------------------------------------------------------------

int Sound_Threshold = 150; 
int32_t ReportingInterval = 1;  // Reporting Interval seconds, unit:0.5s

// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument<256> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[50];

// --------------------------------------------------------------------------------------------
//         receive the data from the nodeRED/dashboard to control the actuators
// --------------------------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) 
{
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");

  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);

  StaticJsonDocument<256> docRcv;
  deserializeJson(docRcv, payload);

  //veify the topic to prove valid message received
  if (0 == strcmp(topic, MQTT_TOPIC_threshold))//topic of setting the sound threshold(mkr1010 with item sensor) 
  {
      Sound_Threshold = docRcv["threshold"];
      Serial.print("Reporting sound threshold has been changed:");
      Serial.println(Sound_Threshold);
//      jsonReceiveDoc.clear();
  } 
  else if (0 == strcmp(topic, MQTT_TOPIC_alarm))//topic of setting the alarm level
  {
      
      Serial.println("alarm received by mkr1010 with item");
   
  }
  else if (0 == strcmp(topic, MQTT_TOPIC_INTERVAL))//topic of setting the interval upload the sound data
  {
      ReportingInterval = docRcv["Interval"];
      Serial.print("Reporting Interval has been changed:");
      Serial.println(ReportingInterval);
  }
  else 
  {
      Serial.println("Unknown command received");
  }
}


// --------------------------------------------------------------------------------------------
//                      put your setup code here, to run once
// --------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  
  // setting the sound sensor as the input
  pinMode(A0,INPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) 
  {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_threshold);//注册连接以接收在给定主题上发布的消息
    mqtt.subscribe(MQTT_TOPIC_alarm);
    mqtt.subscribe(MQTT_TOPIC_INTERVAL);

  } 
  else 
  {
    Serial.println("MQTT Failed to connect!");
  }
}


// --------------------------------------------------------------------------------------------
//                      put your main code here, to run repeatedly
// --------------------------------------------------------------------------------------------
void loop() {
  
  // verify the mqtt connection is still available
  mqtt.loop();//deal with the message which is not completed
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_threshold); //注册连接以接收在给定主题上发布的消息
      mqtt.subscribe(MQTT_TOPIC_alarm);
      mqtt.subscribe(MQTT_TOPIC_INTERVAL);
      mqtt.loop(); //必须定期调用以保持连接正常并获取传入消息吗
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  
  int sound_value=analogRead(A0);

  //   Check if any reads failed and exit early (to try again).
  if (isnan(sound_value)) {
     Serial.println("Failed to read data!");
  } 
  else 
  {
    // only the sound larger than threshold initializa/setting by dashboard
    // send the data
    if(sound_value > Sound_Threshold)
    {
      // Send data to Watson IoT Platform
      status["sound"] = sound_value;
      serializeJson(jsonDoc, msg, 50);
      Serial.println(msg);
    
      if (!mqtt.publish(MQTT_TOPIC, msg)) 
      {
        Serial.println("MQTT Publish failed");
      }  
      Serial.print("Sound Threshold :");
      Serial.print(Sound_Threshold);
      Serial.println();
      Serial.print("ReportingInterval :");
      Serial.print(ReportingInterval);
      Serial.println();
    }

    // Pause - but keep polling MQTT for incoming messages
    // send the data every 0.5 second
    for (int i = 0; i < ReportingInterval; i++) 
    {
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
