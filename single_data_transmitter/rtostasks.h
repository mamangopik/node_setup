void mqtt_sender(void *arguments) {
  vTaskDelay(100 / portTICK_PERIOD_MS);

  const int mqtt_port = 1883;
  String broker = readString(MSTR2);
  broker.trim();
  char buf_broker[100];
  broker.toCharArray(buf_broker, broker.length() + 1);
  client.setServer(buf_broker, mqtt_port);
  boolean res = client.setBufferSize(0xffff - 1);
  String topic = readString(MSTR3);

  while (1) {
    if (!client.connected()) {
      reconnect();
    }

    if (WiFi.status() == WL_DISCONNECTED) {
      ESP.restart();
    }
    client.loop();
    publish_buffer();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void sensor_reader( void * pvParameters ) {
  while(1) {
    read_sensor();
    vTaskDelay(2 / portTICK_PERIOD_MS);
  }
}


void serial_handler(void *arguments) {
  while (1) {
    while (Serial.available() > 1) {
      no_serial_in_wdg = millis();
      char char_in = Serial.read();
      if (char_in != '\n') {
        msg_in += char_in;
      }
      sensor_wdg = millis();
    }

    if (millis() - no_serial_in_wdg > 50 && msg_in.length() > 0) {
      parse_serial();
      msg_in = "";
      Serial.flush();
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void led_status(void *arguments) {
  pinMode(2, OUTPUT);
  while(1){
    if (!client.connected() || WiFi.status() == WL_DISCONNECTED) {
      digitalWrite(2, !digitalRead(2));
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }else{
      digitalWrite(2, !digitalRead(2));
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
