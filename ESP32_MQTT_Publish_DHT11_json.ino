#include <WiFi.h>
#include <PubSubClient.h>

// DHT11 ---------------------------------------
#include "DHT.h"
#define DHTPIN  26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// ----------------------------------------------

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.21";
const char* userId = "ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";

const char* topic = "MyOffice/Outdoor/Value";
const int   PAYLOADSIZE = 64;

WiFiClient wifiClient; 
PubSubClient client(serverIPAddr, 1883, wifiClient);

void setup() {
  Serial.begin(115200);
  Serial.print("\nConnecting to " + String(ssid));
  // 지정한 공유기에 접속 시도
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(" WiFi Connected");   

  Serial.printf("\nConnecting to the Broker(%s)\n", serverIPAddr);
  while ( !client.connect(clientId, userId, userPw) ){ 
    Serial.print("*");
    delay(500);
  }
  Serial.println("Connected to the Broker!!!");  
  // dht11 ---------------------------------
  dht.begin();
}

void loop() {
  char payload[PAYLOADSIZE];
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  String sPayload = "{\"Temp\":" + String(t, 1)
                  + ",\"Humi\":" + String(h, 1) + "}";
  sPayload.toCharArray(payload, PAYLOADSIZE);
  client.publish(topic, payload);
  Serial.print(String(topic) + " ");
  Serial.println(payload); 
  delay(2000);
}
