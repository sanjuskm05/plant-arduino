#include <Wire.h>
#include "DHT.h"

//network and mqtt
#include <WiFi.h>
#include <PubSubClient.h>

#define DHTPIN 15
#define DHTTYPE DHT21   // AM2301 sensor

const char* ssid         = "Francis";
const char* password     = "babybugs";
const char* mqttServer   = "3.89.90.95";
const int   mqttPort     = 1883;
const char* mqttUser     = "mint";
const char* mqttPassword = "mint";
const char* mqttPublish  = "mintpub";
const char* mqttSub      = "mintsub";

WiFiClient espClient;
PubSubClient client(espClient);

#define pump1 19

DHT dht(DHTPIN, DHTTYPE);
 
int clock_frequency = 100000;
int arduino1_add = 11;//slave add is hard coded

long lastMsg = 0;
int msg_delay = 60000;

byte rcv_data;

int light_val = 255;
int soil_val  = 255;

bool is_connected_wifi;
bool is_connected_mqtt;
bool data_error;

//first plant string
String plant_1 = "";

//message back
String messageName = "";
char water;
char light;
  
void setup() {
  //initialize the pump io pin and set high to turn off relay
  pinMode(pump1, OUTPUT);
  digitalWrite(pump1, HIGH);

  //initialize I2C com
  Wire.setClock(clock_frequency);
  Wire.begin();

  //initialize the temp & humidity sensor
  dht.begin();

  //initialize serial com for debugging
  Serial.begin(115200);

  //initialize wifi and connect to network
  WiFi.begin(ssid, password);
  is_connected_wifi = connect_wifi();

  //connect to the mqtt server
  client.setServer(mqttServer, mqttPort);
  client.setCallback(sub_mqtt_server);
  is_connected_mqtt = connect_mqtt_server();


}
 
void loop() {
  
  //reconnect if needed
  if(!is_connected_wifi)
    is_connected_wifi = connect_wifi();
    
  if(!is_connected_mqtt)
    is_connected_mqtt = connect_mqtt_server();
  
  client.loop();

  long now = millis();
  if (now - lastMsg > msg_delay) {
    lastMsg = now;
    plant_1 = "{\"plantName\":\"Pothos\"";
  
    data_error = read_dht();
    data_error += read_plant(1);
  
    if(!data_error && is_connected_mqtt && is_connected_wifi){
      publish_mqtt_msg();
    }
  }

  //delay(msg_delay);        
}

void publish_mqtt_msg(){
  char msg[plant_1.length()];
  plant_1.toCharArray(msg,plant_1.length());
  Serial.print("Sending message to MQTT server ");
  Serial.print(mqttServer);
  Serial.print(", published to \"");
  Serial.print(mqttPublish);
  Serial.println("\"");
  Serial.println(msg);
  Serial.println("\n\n");
  client.publish(mqttPublish, msg);
}

//request data from specified plant
int read_plant(int plant){
  float light_percent;
  float soil_percent;
  int i = 0;

  switch(plant){
    case 1:
      Wire.requestFrom(arduino1_add, 2);
      break;
    
    default:
      break;
  }
  
  while (Wire.available()) { // slave may send less than requested
    switch (i) {
      case 0:
        light_val = Wire.read();     // receive a byte as character
      break;
      case 1:
        soil_val = Wire.read();     // receive a byte as character
      break;
      default:
        Serial.println("\n\n **** Should not have gotten here. **** \n\n");
        //assume something wrong
        return 1;
      break;
    }
    i++;
  }
  light_percent = ((float)light_val)/2.55;
  soil_percent  = ((float)soil_val)/2.55;
  
  plant_1 += ",\"light\":";
  plant_1 += light_percent;
  plant_1 += ",\"moisture\":";
  plant_1 += soil_percent;
  plant_1 += "} ";

  return 0;
}

//get temp and humidity data, format string to send to the mqtt server
int read_dht(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
    return 1;
  }
  else
  {
    //build the string in format, make sure to send 0.0 if val is 0
    plant_1 += ",\"temperature\":";
    if (t == 0){
      plant_1 += "0.0";
    }
    else{
      plant_1 += String(t);     
    }
    plant_1 += ",\"humidity\":";
    if (h == 0){
      plant_1 += "0.0";
    }
    else{
      plant_1 += String(h);     
    }
  }
  return 0;
}

//Connect to the specified wifi network, returns 1 on success, 0 if timed out
int connect_wifi(){
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    i++;
    if (i > 60)
      return 0;
  } 
  Serial.println("Connected to the WiFi network.");
  return 1;
}

//connect to the specified mqtt server, returns 1 on success, 0 if failed
int connect_mqtt_server(){
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("connected");
      Serial.print("Subscribe to \"");
      Serial.print(mqttSub);
      Serial.print("\" ");
      Serial.println( client.subscribe(mqttSub) );
      return 1;
    }
    else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      return 0;
    }
  }
}

//handle messages from the mqtt server
void sub_mqtt_server(char* topic, byte* message, unsigned int length){
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  
  messageName = "";
  int plantindex = 0;
  int i = 0;
  while ((char)message[i] != ','){
    messageName += (char)message[i];
    i++;
  }
  i++;
  water = (char)message[i];
  i++;
  i++;
  light = (char)message[i];

  Serial.println(messageName);
  Serial.println(water);
  Serial.println(light);

  if (messageName == "Pothos")
    plantindex = 1;
  
  switch(plantindex){
    case 0:
      Serial.println("Failed to get plant id and name matched!");
      break;
    case 1:
      if(water == '1')
        water_plant(1);
      if(light == '1')
        light_plant(1,1);
      if(light == '0')
        light_plant(1,0);
    break;
    default:
    break;
  }
}

void water_plant(int index){
  switch(index){
    case 1:
      digitalWrite(pump1, LOW);
      delay(5000);
      digitalWrite(pump1, HIGH);
    break;

    default:
    break;
  }
}

void light_plant(int index, bool turnOnOff){
  Wire.beginTransmission(arduino1_add); // transmit to device
  Wire.write(turnOnOff);     // sends turnOnOff
  Wire.endTransmission();    // stop transmitting
}
