#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>


// MQTT client

StaticJsonDocument<128> doc; 

const char* mqttServer =  "192.168.184.74";
int mqttPort = 1883;
const char *SSID;//
const char *PWD;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


String dataSent = "";
long last_time = 0;
long last_time2 =0;
char data[100];
String dataIn;
bool dis = false;
int cnt_reset = 0;
String str_ip;

char out[128];
String macAddress;
void conf(){
    SSID = "SIIX-IOT";//
    PWD = "SIIX@2018";  
    Serial.print("Connecting to ");
    Serial.println(SSID);
}

void connectToWiFi() {
  conf();
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("connecting");
    delay(500);
    dis = true;
    if (cnt_reset > 10){
       ESP.restart();
    }
    else{
      cnt_reset +=1;   
    } 
  }
  dis = false;
  str_ip = WiFi.localIP().toString();
  Serial.println("connected");
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  Serial.println("IP Address:  ");
  Serial.println(WiFi.localIP());
  
} 

void callback(char* topic, byte* payload, unsigned int length) {
 // Serial.print("Callback - ");
 // Serial.print("Message:");
  for (int i = 0; i < length; i++) {
   // Serial.print((char)payload[i]);
  }
}

void setupMQTT() {
  Serial.println("mqtt setup");
  mqttClient.setServer(mqttServer, mqttPort);
  Serial.println(mqttServer);
      
  // set the callback function
  mqttClient.setCallback(callback);
}



void reconnect() {
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/swa/commands");
      }
      while (WiFi.status() != WL_CONNECTED) {     
        connectToWiFi();
    }
  }
    
}
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  Serial2.begin(115200);
  connectToWiFi();

  macAddress = WiFi.macAddress();

  setupMQTT();

  pinMode (22, INPUT);
  pinMode (23, INPUT);
  //pinMode (19, INPUT);
  //pinMode (21, INPUT);
   
}

// the loop function runs over and over again forever


void loop() {
  
Serial.println ( digitalRead(22));
Serial.println ( digitalRead(23));
Serial.println ( digitalRead(19));
Serial.println ( digitalRead(21));
delay (1000);


  if (!mqttClient.connected()){
    reconnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - last_time > 1000) {

    doc["macAddress"] = macAddress;
    doc["category"] = "esd";
    doc["dr22"] = digitalRead(22);
    doc["dr23"] = digitalRead(23);
    //doc["dr19"] = digitalRead(19);
    //doc["dr21"] = digitalRead(21);
    serializeJson(doc, out);
    
    //sprintf(data, "%s", data1.c_str());
    mqttClient.publish("esd", out);

    
    last_time = now;
  }
}
