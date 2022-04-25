#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Arduino_MKRENV.h>
#include <utility/wifi_drv.h>

// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// Watson IoT connection details
#define MQTT_HOST "tws4j2.messaging.internetofthings.ibmcloud.com"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "d:tws4j2:mkr1010:9C9C1FC16194"
#define MQTT_USER "use-token-auth"
#define MQTT_TOKEN "mkr1010_9C9C1FC16194"
#define MQTT_TOPIC "iot-2/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "iot-2/cmd/display/fmt/json"
#define MQTT_TOPIC_INTERVAL "iot-2/cmd/interval/fmt/json"
#define MQTT_TOPIC_DELAY "iot-2/cmd/delay/fmt/json"

// Add WiFi connection information
char ssid[] = "SKYXHFHG 2.4Ghz";     //  your network SSID (name) ABC"SKYXHFHG 2.4Ghz"
char pass[] = "s7DVhtX57GbP";  // your network password 123456789"s7DVhtX57GbP"

int WiFistatus = WL_IDLE_STATUS;     // the Wifi radio's status
int currentState; 
int ReportingInterval = 5;
int Delay =1000;
// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------

// MQTT objects
void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument<500> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[500];

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);
  char a=(char)payload[0];
  if (0 == strcmp(topic, MQTT_TOPIC_DISPLAY)) {
       if (a == '1'){
         WiFiDrv::analogWrite(26, 255);
         delay(Delay);
         WiFiDrv::analogWrite(26, 0);
     }
       if (a == '2'){
       tone(2, 1000); // Send 1KHz sound signal...
       delay(Delay);
       noTone(2);     // Stop sound...
       delay(1);
     }
     if (a == '3'){
       tone(2, 1000); // Send 1KHz sound signal...
       delay(Delay);
       noTone(2);     // Stop sound...
       delay(1);
     }
    } 
 if (0 == strcmp(topic, MQTT_TOPIC_DELAY)) { 
   Delay = (a-48)*1000;
   Serial.print(Delay);
  
  } else if (0 == strcmp(topic, MQTT_TOPIC_INTERVAL)) {
     ReportingInterval = a-48;
     Serial.print(ReportingInterval);
    }
  
}

void setup() {
  //Initialize serial and wait for port to open:
  pinMode(A0,INPUT);
  pinMode(2, OUTPUT);
  pinMode(0, INPUT);
  pinMode(4, INPUT);    
  pinMode(3, INPUT); 
  WiFiDrv::pinMode(26, OUTPUT);
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
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);
    mqtt.subscribe(MQTT_TOPIC_INTERVAL);
    mqtt.subscribe(MQTT_TOPIC_DELAY);

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
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.subscribe(MQTT_TOPIC_INTERVAL);
      mqtt.subscribe(MQTT_TOPIC_DELAY);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }

  int sensorValue = digitalRead(4);
  currentState = digitalRead(0);
  int pirStat = digitalRead(3); 

   // read all the sensor values using the library functions
  float light=analogRead(A0);
  // Check if any reads failed and exit early (to try again).
  if (isnan(light)) {
    Serial.println("Failed to read data!");
  } else {
    // Send data to Watson IoT Platform
    status["Light"] = light;
    status["Touched"] = currentState;
    status["Door"] = sensorValue;
    status["Motion"] = pirStat;
    serializeJson(jsonDoc, msg, 500);
    Serial.println(msg);
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }
  }

  // Pause - but keep polling MQTT for incoming messages
  for (int i = 0; i < ReportingInterval; i++) {
    mqtt.loop();
    delay(1000);
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
