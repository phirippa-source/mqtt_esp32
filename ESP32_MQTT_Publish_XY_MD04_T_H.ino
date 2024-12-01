#include <WiFi.h>
#include <PubSubClient.h>

// XY-MD04 ---------------------------------------
#define RS485RW     4
#define WRITE_MODE  HIGH
#define READ_MODE   LOW

byte cmd[] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02, 0x20, 0x0b};
HardwareSerial RS485(2); // Serial2를 RS485로 지정
// ----------------------------------------------

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "mqtt_ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topicTemp = "MyOffice/Outdoor/Temp";
const char* topicHumi = "MyOffice/Outdoor/Humi";

WiFiClient wifiClient; 
PubSubClient client(serverIPAddr, 1883, wifiClient);

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
    // XY-MD04
    RS485.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(RS485RW, OUTPUT);  
}

void loop() {
    float t, h;
    byte rxBuf[9];
    char buf[8];

    digitalWrite(RS485RW, WRITE_MODE);
    RS485.write(cmd, sizeof(cmd));
    RS485.flush();
    digitalWrite(RS485RW, READ_MODE);

    if(RS485.readBytes(rxBuf, 9)) {
        t = ((rxBuf[3] << 8) | rxBuf[4]) / 10.0;
        h = ((rxBuf[5] << 8) | rxBuf[6]) / 10.0;
    
        sprintf(buf, "%.1f", t);
        client.publish(topicTemp, buf);
        Serial.println(String(topicTemp) + " : " + String(buf));
        sprintf(buf, "%.1f", h);
        client.publish(topicHumi, buf);
        Serial.println(String(topicHumi) + " : " + String(buf));
    }
    delay(2000);
}
