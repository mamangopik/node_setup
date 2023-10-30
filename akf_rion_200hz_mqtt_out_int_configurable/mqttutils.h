void reconnect() {
  while (!client.connected()) {
    String channel = String(WiFi.macAddress());
    if (client.connect(channel.c_str())) {
      Serial.println("{\"SUCCESS\":\"broker connected\"}");
    } else {
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

void publish_buffer(byte buffer_loc) {
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
  json_data += "}";

  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);
  
  client.publish(buf_sensor_topic, json_data.c_str());
  id_data++;
}