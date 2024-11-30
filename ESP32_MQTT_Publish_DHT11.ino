#include <WiFi.h>
#include <PubSubClient.h>

// DHT11 ---------------------------------------
#include "DHT.h"
#define DHTPIN  4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
// ----------------------------------------------

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "mqtt_ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topic_t = "MyOffice/Outdoor/Temp";
const char* topic_h = "MyOffice/Outdoor/Humi";

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
    char buf[8];
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t) ) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    sprintf(buf, "%.1f", t);
    client.publish(topic_t, buf);
    Serial.println(String(topic_t) + " : " + String(buf));

    sprintf(buf, "%.1f", h);
    client.publish(topic_h, buf);
    Serial.println(String(topic_h) + " : " + String(buf));
    delay(2000);
}
