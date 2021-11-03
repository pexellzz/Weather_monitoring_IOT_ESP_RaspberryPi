#include <WiFi.h>
const char* ssid = "HUAWEI Y9s";
const char* password = "grey1sucksass2";
WiFiClient espClient;

#include <PubSubClient.h>
const char* mqtt_server = "192.168.43.62";
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <SPI.h>
#define BMP_SCK 18
#define BMP_MISO 19
#define BMP_MOSI 23
#define BMP_CS 5
Adafruit_BMP280 bmp; // I2C
#include "DHT.h" 
#define dht_dpin 2 
#define DHTTYPE DHT11   // DHT 11      
DHT dht(dht_dpin, DHTTYPE); 


float temperature = 0.0;
float humidite = 0.0;
float pressure =0.0;
float altitude =0.0;
char* topic;
char* messageTemp;
// LED Pin
const int ledPin = 4;

void setup() {
  Serial.begin(9600);

  //status = bme.begin();  
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println(" BME280 sensor detected");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "Room1/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Room1/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    
    //  ************* Temperature in Celsius *************
    
    temperature = bmp.readTemperature();   
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    client.publish("Room1/temperature", tempString);

    //  ************* Pression en pascal *************

    pressure = bmp.readPressure();
    char presString[8];
    dtostrf(pressure, 1, 2, presString);
    Serial.print("Pression: ");
    Serial.println(presString);
    client.publish("Room1/pressure", presString);

    //  ************* humidite en % *************

    //humidite = dht.readHumidity();
    float h = dht.readHumidity();
    //char humString[8];
    //dtostrf(humidite, 1, 2, humString);
    char message[10]="" ;
    Serial.print("Humidité: ");
    sprintf(message,"%f",h);
    //Serial.println(humString);
    client.publish("Room1/humidity",message);  

    //  ************* Altitude en Metre *************

    altitude = bmp.readAltitude(1013.0);
    char altString[8];
    dtostrf(altitude, 1, 2, altString);
    Serial.print("Altitude: ");
    Serial.println(altString);
    client.publish("Room1/altitude", altString);  
  }
  client.subscribe("Room1/output");
  if (String(topic) == "Room1/output") {
  Serial.print("Changing output to ");
  if (messageTemp == "on") {
    Serial.println("on");
    digitalWrite(ledPin, HIGH);
  }
  else if (messageTemp == "off") {
    Serial.println("off");
    digitalWrite(ledPin, LOW);
  }
}
}
