#include "header.h"

void setup() {
  esp_task_wdt_init(0xffffffff, false);
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xTaskCreatePinnedToCore(
    serial_handler,   /* Task function. */
    "serial handler",     /* name of task. */
    2048,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */

  xTaskCreatePinnedToCore(
    led_status, 
    "led status",
    1024, 
    NULL, 
    8, 
    NULL, 
    1); 
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
    if (millis() - wlan_timer > 20000) {
      // Serial.println("WiFi Error");
      ESP.restart();
    }
  }
  // Serial.println("Connected to WiFi");
  vTaskDelay(1000 / portTICK_PERIOD_MS);  Serial.begin(115200);


  xTaskCreatePinnedToCore(
    sensor_reader, 
    "sensor reader",
    10000,
    NULL,
    1,
    &Task2,
    0); 

  xTaskCreatePinnedToCore(
    mqtt_sender, 
    "mqtt sender",
    4096, 
    NULL, 
    1, 
    NULL,
    1); 
}

void loop() {
}