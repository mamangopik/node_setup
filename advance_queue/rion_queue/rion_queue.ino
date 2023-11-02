#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// user typedef
#define RXD2 16
#define TXD2 17

WiFiClient espClient;
PubSubClient client(espClient);

// global variables 
byte counter = 0;
byte found = 0;
byte command[14];
byte header[4];
byte checksum[1];
byte buffer_mon = 0;
byte buffer_0_ready = 0;
byte buffer_1_ready = 0;

String msg_in = "";
String sensor_topic = "";
String raw = "";

const int DATA_SIZE = 64;

int x_values[2][DATA_SIZE];
int y_values[2][DATA_SIZE];
int z_values[2][DATA_SIZE];
int data_count = 0;
int last_ts = 0;

const int maxArraySize = 200;
String dataArray[maxArraySize];
int arraySize = 0;  


unsigned long start_time = 0;
unsigned long wlan_timer = 0;
unsigned long sensor_wdg = 0;
unsigned long ts_timer = 0;
unsigned long id_data = 0;
unsigned long id_data_sent = 0;
unsigned long no_serial_in_wdg = 0;

float v_batt = 0.0;

// Sensor and hardware control functions
void cek_sensor();
void set_autorate();

// Data processing and parsing functions
void process_command();
int createXRaw(byte command[]);
int createYRaw(byte command[]);
int createZRaw(byte command[]);
int calculateValue(uint8_t nibbles[]);
unsigned char calculateChecksum(unsigned char command[], unsigned char length);

#include "sensorutils.h"

void addDataToArray(const char* jsonString) {
  if (arraySize < maxArraySize) {
    dataArray[arraySize] = jsonString;
    arraySize++;
    id_data++;
  }
}

void removeDataFromArray() {
  if (arraySize > 0) {
    unsigned long t0=millis();
    // Shift elements to the left to remove the first element
    for (int i = 0; i < arraySize - 1; i++) {
      dataArray[i] = dataArray[i + 1];
    }
    arraySize--;
    Serial.print("removing time:");Serial.print(millis()-t0);Serial.println(" ms");
  }
}

int readSensor() {
  if (buffer_0_ready == 1) {
      addDataToArray(sensor_to_json(0).c_str());
      buffer_0_ready = 0;
  }
  if (buffer_1_ready == 1) {
      addDataToArray(sensor_to_json(1).c_str());
      buffer_1_ready = 0;
  }
  if(buffer_0_ready==1||buffer_1_ready==1){
    return 1;
  }else{
    return -1;
  }
}

void sensor_reader(void *arguments) {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  set_autorate();
  while (1) {
    while (Serial2.available() > 0) {
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
        process_command(); //isi buffer 0 dan 1
      }
    }
    // Serial.println("Reader");
  }
}

int send_data(const char* jsonString){
    if (client.publish("/shms/node_3/accelerometer", jsonString)) {
      Serial.println("{\"SUCCESS\":\"Message sent\"}");
      return 1;
    } else {
      Serial.println("{\"ERR\":\"Message doesn't sent\"}");
      return -1;
    }
}

void reconnect() {
  while (!client.connected()) {
    String channel = String(WiFi.macAddress());
    if (client.connect(channel.c_str())) {
      Serial.println("{\"SUCCESS\":\"broker connected\"}");
    } else {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}

void mqtt_sender(void *arguments) {
  unsigned long sender_timer = millis();
  client.setServer("brains.local", 1883);
  boolean res = client.setBufferSize(0xffff - 1);
  while (1) {
    if(dataArray[0].length()>100 && id_data_sent!=id_data){
        bool messageSent = false;
        while (!messageSent) { // Retry up to 3 times
            if (client.publish("/shms/node_3/accelerometer", dataArray[0].c_str())) {
                Serial.println("Message sent successfully");
                removeDataFromArray();
                messageSent = true; // Set to true to exit the retry loop
                id_data_sent = id_data;
            } else {
                Serial.println("Message failed to send. Retrying...");
                // Delay before retrying
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            Serial.print("data id:");Serial.println(id_data);
        }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void connection_manager(void *arguments) {
  while (1) {
    if (!client.connected()) {
        Serial.println("{\"INFO\":\"broker dis-connected\"}");
        reconnect();
    }
    client.loop();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}


void data_manager(void *arguments) {
  while (1) {
    if(readSensor()) {//read sensor and push it to data queue
        // Serial.println("data pushed to queue");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }else{
        // Serial.println("waiting buffer");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

void setup() {
  esp_task_wdt_init(0xffffffff, false);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  wlan_timer = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin("test-ADSB", "nutech2020");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - wlan_timer > 10000) {
      Serial.println("{\"ERR\":\"WiFi Error\"}");
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      ESP.restart();
    }
  }
  Serial.println("{\"SUCCESS\":\"Connected to WiFi\"}");
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
    sensor_reader,
    "sensor reader",
    2048,
    NULL,
    1,
    NULL,
    0);

  xTaskCreatePinnedToCore(
    data_manager,
    "data manager",
    2048,
    NULL,
    2,
    NULL,
    1);

  xTaskCreatePinnedToCore(
    mqtt_sender,
    "mqtt sender",
    2048,
    NULL,
    0,
    NULL,
    1);

  xTaskCreatePinnedToCore(
    connection_manager,
    "mqtt connection_manager",
    2048,
    NULL,
    3,
    NULL,
    1);
}

void loop() {
    Serial.print("arraySize ");Serial.println(arraySize);
    Serial.println("\"INFO\":\"[APP] Free memory: " + String(esp_get_free_heap_size()/1024) + " KBytes\"");
    vTaskDelay(5000 / portTICK_PERIOD_MS);
}
