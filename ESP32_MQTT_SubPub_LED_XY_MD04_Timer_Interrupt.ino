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

hw_timer_t *timer0 = NULL;
volatile bool has_expired = false;

void IRAM_ATTR isr_timer0Interrupt() {
    has_expired = true;
}

void on_message(char* topic, byte* payload, unsigned int length) { 
    char msgBuf[64];
    Serial.printf("Message arrived!\nTopic: %s\nLen. of payload: %d\n", 
                    topic, length);
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

void pubXY_MD04(void) {
    char buf[8];
    float t, h;
    byte rxBuf[10];

    digitalWrite(RS485RW, WRITE_MODE);
    RS485.write(cmd, sizeof(cmd));
    RS485.flush();
    digitalWrite(RS485RW, READ_MODE);
    if(RS485.readBytes(rxBuf, 9)) {
        t = ((rxBuf[3] << 8) | rxBuf[4]) / 10.0;
        h = ((rxBuf[5] << 8) | rxBuf[6]) / 10.0;
        
        sprintf(buf, "%.1f", t);
        client.publish(topicTemp, buf);
        Serial.print(topicTemp); Serial.print(" : "); Serial.println(buf);
        
        sprintf(buf, "%.1f", h);
        client.publish(topicHumi, buf);
        Serial.print(topicHumi); Serial.print(" : "); Serial.println(buf);
    } 
}
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
    // Timer Interrupt
    timer0 = timerBegin(1000000); 
    timerAttachInterrupt(timer0, &isr_timer0Interrupt); 
    timerAlarm(timer0, 1000000*10, true, 0);
}
unsigned long int preTime = 0;

void loop() {
    client.loop();
    if (has_expired) {
        has_expired = false;
        pubXY_MD04();
    }    
}
