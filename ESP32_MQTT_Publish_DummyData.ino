


void loop() {
    client.publish(topic, "27.32");
    Serial.println(String(topic) + " : 27.32");
    delay(2000);
 }
