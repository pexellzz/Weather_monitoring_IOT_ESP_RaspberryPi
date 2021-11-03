#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11
#define watersensor 5
#define dht_dpin 0

const char* ssid = "HUAWEI Y9s";                   // wifi ssid
const char* password =  "grey1sucksass2";         // wifi password
const char* mqttServer = "192.168.43.62";    // IP adress Raspberry Pi
const int mqttPort = 1883;
const char* mqttUser = "";      // if you don't have MQTT Username, no need input
const char* mqttPassword = "";  // if you don't have MQTT Password, no need input

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(dht_dpin, DHTTYPE);

void setup() {

  Serial.begin(115200);
  dht.begin();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }

  //  client.publish("esp8266", "Hello Raspberry Pi");
  //  client.subscribe("esp8266");

}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  char humidite[10]="" ;
  char température[10]="" ;
  //sprintf(message, "%f/%f",h,t);
  sprintf(humidite,"%f",h);
  sprintf(température,"%f",t);
  //char message[]="hello" ;
  //dtostrf(temp, 4, 2, str_temp);
  client.publish("R2/humidité",humidite);
  client.publish("R2/température",température);
  client.subscribe("R2/humidité");
  client.subscribe("R2/température");
  delay(800);
  client.loop();
}
