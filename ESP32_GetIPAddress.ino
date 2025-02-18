#include <WiFi.h>
const char* ssid = "AP_NAME";         // AP_NAME  : 공유기 이름
const char* password = "PASSWORD";    // PASSWORD : 패스워드

void setup() {
    Serial.begin(115200);
    Serial.print("\nConnecting to " + String(ssid));

    // 해당 공유기에 접속 시도
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println(" WiFi Connected");

    // ESP32 보드가 공유기로부터 할당 받은 "(사설)IP 주소"
    Serial.print("Local IP address : " + WiFi.localIP().toString());
}

void loop() {
}

