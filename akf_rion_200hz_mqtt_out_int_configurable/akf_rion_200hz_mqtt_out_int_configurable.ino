#include "header.h"

void mqtt_sender(void *arguments) {
  vTaskDelay(100 / portTICK_PERIOD_MS);

  //  const char* mqtt_server = readString(MSTR2).c_str();
  //  const char* mqtt_server = "broker.hivemq.com";
  const int mqtt_port = 1883;
  //  const char* mqtt_topic = readString(MSTR0).c_str();

  String broker = readString(MSTR2);
  broker.trim();
  char buf_broker[100];
  broker.toCharArray(buf_broker, broker.length() + 1);


  client.setServer(buf_broker, mqtt_port);
  boolean res = client.setBufferSize(0xffff - 1);
  if (res) Serial.println("Buffer resized.");
  else Serial.println("Buffer resizing failed");

  String topic = readString(MSTR3);

  while (1) {
    if (!client.connected()) {
      reconnect();
    }

    if (WiFi.status() == WL_DISCONNECTED) {
      ESP.restart();
    }
    client.loop();

    if (buffer_0_ready == 1) {
      publish_buffer(0);
      buffer_0_ready = 0;
    }
    if (buffer_1_ready == 1) {
      publish_buffer(1);
      buffer_1_ready = 0;
    }
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
    }

    if (millis() - no_serial_in_wdg > 50 && msg_in.length() > 0) {
      Serial.println(msg_in);
      parse_serial();
      msg_in = "";
      Serial.flush();
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void setup() {
  esp_task_wdt_init(0xffffffff, false);
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  pinMode(2, OUTPUT);

  xTaskCreatePinnedToCore(
    serial_handler,   /* Task function. */
    "serial handler",     /* name of task. */
    2048,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  vTaskDelay(1000 / portTICK_PERIOD_MS);


  wlan_timer = millis();
  String ssid = readString(MSTR0);
  String password = readString(MSTR1);
  sensor_topic = readString(MSTR3);
  
  ssid.trim();
  password.trim();
  char buf_SSID[100];
  char buf_PWD[100];
  ssid.toCharArray(buf_SSID, ssid.length() + 1);
  password.toCharArray(buf_PWD, password.length() + 1);
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(buf_SSID, buf_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(2, !digitalRead(2));
    Serial.print("Connecting to "); Serial.println(buf_SSID);
    Serial.print("with password "); Serial.println(buf_PWD);
    if (millis() - wlan_timer > 20000) {
      Serial.println("WiFi Error");
      ESP.restart();
    }
  }
  Serial.println("Connected to WiFi");
  digitalWrite(2, 1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
    Task2code,   /* Task function. */
    "Task2",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &Task2,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */

  xTaskCreatePinnedToCore(
    mqtt_sender,   /* Task function. */
    "mqtt sender",     /* name of task. */
    4096,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
}

void loop() {
}
