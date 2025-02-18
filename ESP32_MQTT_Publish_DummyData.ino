#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "AP_NAME";
const char* password = "PASSWORD";
const char* serverIPAddr = "IP Address of MQTT Broker";
const char* userId = "username";
const char* userPw = "password";
const char* clientId = "unique client ID";

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
    // ESP32 보드가 공유기로부터 할당 받은 "(사설)IP 주소"
    Serial.println("Local IP address : " + WiFi.localIP().toString());

    Serial.printf("\nConnecting to the Broker(%s)\n", serverIPAddr);
    while ( !client.connect(clientId, userId, userPw) ){ 
        Serial.print("*");
        delay(500);
    }
    Serial.println("Connected to the Broker!!!");
}

void loop() {
    client.publish(topic, "27.32");
    Serial.println(String(topic) + " : 27.32");
    delay(2000);
}
