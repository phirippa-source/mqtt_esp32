#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topic = "MyOffice/Outdoor/Command";
char msgBuf[64];

void on_message(char* topic, byte* payload, unsigned int length) { 
    Serial.printf("Message arrived!\nTopic: %s\nLen. of payload: %d\n", topic, length);
    strncpy(msgBuf, (char*)payload, length);
    Serial.printf("Payload: %s\n", msgBuf);
}

WiFiClient wifiClient; 
PubSubClient client(serverIPAddr, 1883, on_message, wifiClient);

void setup() {
    Serial.begin(115200);
   
    Serial.print("\nConnecting to " + String(ssid));
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
    
    Serial.println("Subscribing! topic = %s", topic);
    client.subscribe(topic);
}

void loop() {
    client.loop();
}
