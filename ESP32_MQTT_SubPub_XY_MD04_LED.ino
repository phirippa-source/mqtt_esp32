#include <WiFi.h>
#include <PubSubClient.h>

#define RS485RW     4
#define WRITE_MODE  HIGH
#define READ_MODE   LOW

HardwareSerial RS485(2);       
byte cmd[] = {0x01, 0x04, 0x00, 0x01, 0x00, 0x02, 0x20, 0x0b};

const char* ssid = "Ria2G";
const char* password = "730124go";
const char* serverIPAddr = "192.168.0.16";
const char* userId = "mqtt_ship";
const char* userPw = "1234";
const char* clientId = "unique client ID";
const char* topicSub = "MyOffice/Outdoor/Led";
const char* topicTemp = "MyOffice/Outdoor/Temp";
const char* topicHumi = "MyOffice/Outdoor/Humi";

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
        Serial.print(".");  delay(500);
    }
    Serial.println(" WiFi Connected");   

    Serial.printf("\nConnecting to the Broker(%s)\n", serverIPAddr);
    while ( !client.connect(clientId, userId, userPw) ){ 
        Serial.print("*");  delay(500);
    }
    Serial.println("Connected to the Broker!!!");  
    
    Serial.printf("Subscribing! topic = %s\n", topicSub);
    client.subscribe(topicSub);

    // GPIO pin setup & XY-MD04
    pinMode(LED_BUILTIN, OUTPUT);
    RS485.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(RS485RW, OUTPUT);   
}

void loop() {
    char buf[8];
    float t, h;
    byte rxBuf[10];

    client.loop();

    digitalWrite(RS485RW, WRITE_MODE);
    RS485.write(cmd, sizeof(cmd));
    RS485.flush();
    digitalWrite(RS485RW, READ_MODE);
    if(RS485.readBytes(rxBuf, 9)) {
        t = ((rxBuf[3] << 8) | rxBuf[4]) / 10.0;
        h = ((rxBuf[5] << 8) | rxBuf[6]) / 10.0;

        Serial.println("Temp : " + String(t, 1) + "\tHumi : " + String(h, 1));
        
        sprintf(buf, "%.1f", t);
        client.publish(topicTemp, buf);
        Serial.print(topicTemp); Serial.print(" : "); Serial.println(buf);
        
        sprintf(buf, "%.1f", h);
        client.publish(topicHumi, buf);
        Serial.print(topicHumi); Serial.print(" : "); Serial.println(buf);
    } 
    delay(4000);
}
