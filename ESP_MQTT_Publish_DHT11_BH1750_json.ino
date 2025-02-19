#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <Wire.h>
#include <BH1750.h>
#define DHTPIN 26
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.21";
const char* userId = "ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char *topic = "MyOffice/Indoor/Value";
const int PAYLOADSIZE = 128;

WiFiClient wifiClient; 
PubSubClient client(serverIPAddr, 1883, wifiClient);

void setup() {
  dht.begin();
  Wire.begin();
  lightMeter.begin();
  Serial.begin(115200);
  Serial.print("\nConnecting to "); Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("."); delay(500);
  }
  Serial.println("\nWiFi Connected");

  Serial.print("Connecting to broker(" + String(serverIPAddr) + ")");
  while ( !client.connect(clientId, userId, userPw) ){ 
    Serial.print("*"); delay(500);
  }
  Serial.println("\nConnected to broker!!!");
}

void loop() {
  char payload[PAYLOADSIZE];
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");  
    return;
  }
  float lux = lightMeter.readLightLevel();
  // sPayload에 저장되는 문자열을 python 문법으로 표현하면
  // '{"Temp": 23.1, "Humi": 34.1, "Lux":341}'
  String sPayload = "{\"Temp\":"   + String(t, 1)
                    + ",\"Humi\":" + String(h, 1)
                    + ",\"Lux\":"   + String(lux, 1)
                    + "}";

  sPayload.toCharArray(payload, PAYLOADSIZE);
  client.publish(topic, payload);
  Serial.print(String(topic) + " "); 
  Serial.println(payload); 
  delay(2000);
}
