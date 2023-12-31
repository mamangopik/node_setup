void reconnect() {
  while (!client.connected()) {
    led_status_mode = DISCONNECTED;
    String channel = String(WiFi.macAddress());
    if (client.connect(channel.c_str())) {
      led_status_mode = CONNECTED;
      Serial.println("{\"SUCCESS\":\"broker connected\"}");
    } else {
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

void publish_buffer(byte buffer_loc) {
#ifdef VSENSE_PIN
  String json_data = "{";
  json_data += "\"x_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(x_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],\"y_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(y_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],\"z_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(z_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],";
  json_data += "\"sensor_type\":";
  json_data += '"';
  json_data += "accelerometer";
  json_data += '"';
  json_data += ",\"battery_voltage\":"+String(v_batt);
  json_data += ",\"signal_strength\":"+String(WiFi.RSSI());
  json_data += "}";
#endif
#ifndef VSENSE_PIN
  String json_data = "{";
  json_data += "\"x_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(x_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],\"y_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(y_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],\"z_values\":[";
  for (int i = 0; i < DATA_SIZE; i++) {
    json_data += String(z_values[buffer_loc][i]);
    if (i != DATA_SIZE - 1) {
      json_data += ",";
    }
  }
  json_data += "],";
  json_data += "\"sensor_type\":";
  json_data += "\"accelerometer\"";
  json_data += ",\"signal_strength\":"+String(WiFi.RSSI());
  json_data += "}";
#endif


  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);

  if (client.publish(buf_sensor_topic, json_data.c_str())) {
    Serial.println("{\"SUCCESS\":\"Message sent\"}");
  } else {
    Serial.println("{\"ERR\":\"Message doesn't sent\"}");
  }
  //  id_data++;
}
