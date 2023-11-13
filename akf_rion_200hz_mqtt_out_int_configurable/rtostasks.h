void mqtt_sender(void *arguments) {
  vTaskDelay(100 / portTICK_PERIOD_MS);
  const int mqtt_port = 1883;
  String broker = readString(MSTR2);
  broker.trim();
  char buf_broker[100];
  broker.toCharArray(buf_broker, broker.length() + 1);
  String topic = readString(MSTR3);
  client.setTimeout(7000);
  client.begin(buf_broker, net);
  while (1) {

    client.loop();
    if (!client.connected()) {
      connect();
    }

    if (WiFi.status() == WL_DISCONNECTED) {
      Serial.println("{\"ERR\":\"WiFi Error\"}");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      ESP.restart();
    }

    if (buffer_0_ready == 1 && client.connected()) {
      // suspend tasks that run on core 1
      vTaskSuspend(LED_TASK);
      vTaskSuspend(SERIAL_TASK);
      publish_buffer(0);
      // resume tasks that run on core 1
      vTaskResume(LED_TASK);
      vTaskResume(SERIAL_TASK);
      buffer_0_ready = 0;
    }
    if (buffer_1_ready == 1 && client.connected()) {
      // resume tasks that run on core 1
      vTaskSuspend(LED_TASK);
      vTaskSuspend(SERIAL_TASK);
      publish_buffer(1);
      // resume tasks that run on core 1
      vTaskResume(LED_TASK);
      vTaskResume(SERIAL_TASK);
      buffer_1_ready = 0;
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
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
  pinMode(LEDSTATUSPIN, OUTPUT);
  while (1) {
    if (!client.connected() || WiFi.status() == WL_DISCONNECTED) {
      digitalWrite(LEDSTATUSPIN, !digitalRead(LEDSTATUSPIN));
      vTaskDelay(100 / portTICK_PERIOD_MS);
    } else {
      digitalWrite(LEDSTATUSPIN, 1);
      vTaskDelay(10 / portTICK_PERIOD_MS);
      digitalWrite(LEDSTATUSPIN, 0);

      digitalWrite(LEDSTATUSPIN, 0);
      vTaskDelay(3000 / portTICK_PERIOD_MS);

    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
    Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()) + " bytes");
  }

}

void sensor_reader( void * pvParameters ) {
  last_ts = 0;
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  sensor_wdg = millis();
  set_autorate();
  for (;;) {
    cek_sensor();
    while (Serial2.available() > 0) {
      cek_sensor();
      byte in = Serial2.read();
      if (in == 104 && found == 0) {
        found = 1;
        ts_timer = micros();
      }
      if (found == 1) {
        raw += String(in, HEX);
        command[counter] = in;
        counter += 1;
      }
      if (counter == 14) {
        process_command();
      }
    }
  }
}
