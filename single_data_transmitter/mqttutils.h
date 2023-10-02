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
  String json_data;
  json_data = String(random(31,32));

  sensor_topic.trim();
  char buf_sensor_topic[100];
  sensor_topic.toCharArray(buf_sensor_topic, sensor_topic.length() + 1);
  
  client.publish(buf_sensor_topic, json_data.c_str());
}