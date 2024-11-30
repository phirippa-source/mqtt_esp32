#include <WiFi.h>
#include <PubSubClient.h>

// BH1750 ---------------------------------------
#include <Wire.h>
#include <BH1750.h>
BH1750 lightMeter;
// ----------------------------------------------

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "mqtt_ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topic = "MyOffice/Outdoor/Lux";

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
    
    // BH1750 ---------------------------------
    Wire.begin();
    lightMeter.begin();
}

void loop() {
    char buf[8];
    float lux = lightMeter.readLightLevel();
    sprintf(buf, "%.1f", lux);
    client.publish(topic, buf);
    Serial.println( String(topic) + " : " + String(buf) );
    delay(2000);
}

