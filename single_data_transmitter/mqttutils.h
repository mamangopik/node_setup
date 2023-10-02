void reconnect() {
  while (!client.connected()) {
    String channel = String(WiFi.macAddress());
    if (client.connect(channel.c_str())) {
      // Serial.println("broker connected");
    } else {
      vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
  }
}

void publish_buffer() {
  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);

  String json_data;
  json_data = "{\"id\":"+String(id_data)+",\"sensor_type\":\"single_data\",\"value\":" + String(random(25,32)) + ",\"label\":\"temperature\"}";
  client.publish(buf_sensor_topic, json_data.c_str());
  id_data ++;
}