#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topic = "MyOffice/Outdoor/Led";

void on_message(char* topic, byte* payload, unsigned int length) { 
    char msgBuf[64];
    Serial.printf("Message arrived!\nTopic: %s\nLen. of payload: %d\n", topic, length);
    strncpy(msgBuf, (char*)payload, length);
    msgBuf[length] = '\0';
    String led = String(msgBuf);
    Serial.println("Payload: " + led);
    
    if (led == "1" || led == "on" || led == "ON") {
        digitalWrite(LED_BUILTIN, HIGH);
    } else if (led == "0" || led == "off" || led == "OFF"){
        digitalWrite(LED_BUILTIN, LOW);
    } else {
        Serial.println("Wrong Message! : " + led );
    }
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
    
    Serial.printf("Subscribing! topic = %s", topic);
    client.subscribe(topic);

    // GPIO pin setup
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    client.loop();
}
