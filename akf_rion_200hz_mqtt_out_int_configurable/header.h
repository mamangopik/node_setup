#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_task_wdt.h>
#include <EEPROM.h>

#define RXD2 16
#define TXD2 17
#define EEPROM_SIZE 1024
#define MSTR0 100
#define MSTR1 200
#define MSTR2 300
#define MSTR3 400
#define MSTR4 500


//("SSID:")(readString(MSTR3));
//("WIFI_PASS:")(readString(MSTR1));
//("BROKER:")(readString(MSTR2));
//("TOPIC:")(readString(MSTR0));

TaskHandle_t Task2;
WiFiClient espClient;
PubSubClient client(espClient);

String raw = "";
byte counter = 0;
byte found = 0;
byte command[14];
byte header[4];
byte checksum[1];

String msg_in = "";
String sensor_topic = "";

const int DATA_SIZE = 128;
int x_values[2][DATA_SIZE];
int y_values[2][DATA_SIZE];
int z_values[2][DATA_SIZE];
int data_count = 0;
unsigned long start_time = 0;
unsigned long wlan_timer = 0;
unsigned long sensor_wdg = 0;
unsigned long ts_timer = 0;
unsigned long id_data = 0;
unsigned long no_serial_in_wdg = 0;


int last_ts = 0;

byte buffer_mon = 0, buffer_0_ready = 0, buffer_1_ready = 0;

int createXRaw(byte command[]) {
  int x_raw = 0;
  for (byte i = 4; i <= 6; i++) {
    x_raw <<= 8;
    x_raw |= command[i];
  }
  return x_raw;
}

int createYRaw(byte command[]) {
  int y_raw = 0;
  for (byte i = 7; i <= 9; i++) {
    y_raw <<= 8;
    y_raw |= command[i];
  }
  return y_raw;
}

int createZRaw(byte command[]) {
  int z_raw = 0;
  for (byte i = 10; i <= 12; i++) {
    z_raw <<= 8;
    z_raw |= command[i];
  }
  return z_raw;
}

int calculateValue(uint8_t nibbles[]) {
  uint16_t x_sign = nibbles[5];
  uint16_t x_num = (nibbles[4] * 10) + nibbles[3];
  uint16_t x_prec = (nibbles[2] * 100) + (nibbles[1] * 10) + nibbles[0];

  int value = (x_num * 1000) + (x_prec);
  if (x_sign > 0) {
    value *= -1;
  }

  return value;
}

unsigned char calculateChecksum(unsigned char command[], unsigned char length) {
  unsigned char sum = 0;
  for (unsigned char i = 1; i < length; i++) {
    sum += command[i];
  }
  sum &= 0xFF; // Masking the sum to ensure it fits within a byte
  return sum;
}

void process_command() {
  last_ts = micros() - ts_timer;
  sensor_wdg = millis();
  int x_raw = createXRaw(command);
  int y_raw = createYRaw(command);
  int z_raw = createZRaw(command);
  checksum[0] = command[13];
  raw = "";
  counter = 0;
  found = 0;

  uint8_t x_nibbles[6];
  for (byte i = 0; i < 6; i++) {
    x_nibbles[i] = (x_raw >> (i * 4)) & 0x0F;
  }
  int x_value = calculateValue(x_nibbles);

  uint8_t y_nibbles[6];
  for (byte i = 0; i < 6; i++) {
    y_nibbles[i] = (y_raw >> (i * 4)) & 0x0F;
  }
  int y_value = calculateValue(y_nibbles);

  uint8_t z_nibbles[6];
  for (byte i = 0; i < 6; i++) {
    z_nibbles[i] = (z_raw >> (i * 4)) & 0x0F;
  }
  int z_value = calculateValue(z_nibbles);

  byte calculated_checksum = calculateChecksum(command, 13);
  if (calculated_checksum == checksum[0]) {
    if (x_value >= -4000 && x_value <= 4000 && y_value >= -4000 && y_value <= 4000 && z_value >= -4000 && z_value <= 4000) {
      // Store data in arrays
      if (buffer_mon == 0) {
        x_values[0][data_count] = x_value;
        y_values[0][data_count] = y_value;
        z_values[0][data_count] = z_value;
        data_count++;
      }
      if (buffer_mon == 1) {
        x_values[1][data_count] = x_value;
        y_values[1][data_count] = y_value;
        z_values[1][data_count] = z_value;
        data_count++;
      }
    }
  }
  if (data_count == DATA_SIZE) {
    //switch buffer
    if (buffer_mon == 0) {
      buffer_0_ready = 1;
      buffer_mon = 1;
      //            Serial.println ("buffer 0 rady, saving on buffer 1");
    }
    else {
      buffer_1_ready = 1;
      buffer_mon = 0;
      //            Serial.println ("buffer 1 rady, saving on buffer 0");
    }
    data_count = 0; // Reset arrays
    unsigned long duration = millis() - start_time;
    // Serial.print("Data collection duration: ");
    // Serial.print(duration);
    // Serial.println(" ms");
    // Serial.print("T sampling: ");
    // Serial.print((float)(duration / 256));
    // Serial.println(" ms");
    String timedata = String(duration);
    start_time = millis(); // Start new time measurement
  }
}

void set_autorate() {
  delay(500);
  Serial2.updateBaudRate(9600);
  delay(500);
  byte command_changebaud[6] = {0x68, 0x05, 0x00, 0x0b, 0x05, 0xff};
  command_changebaud[5] = calculateChecksum(command_changebaud, 5);
  for (byte i = 0; i < 6; i++) {
    Serial2.write(command_changebaud[i]);
  }
  vTaskDelay(500 / portTICK_PERIOD_MS);
  Serial2.updateBaudRate(115200);
  vTaskDelay(500 / portTICK_PERIOD_MS);
  byte command_autorate[6] = {0x68, 0x05, 0x00, 0x0c, 0x06, 0xff};
  command_autorate[5] = calculateChecksum(command_autorate, 5);
  for (byte i = 0; i < 6; i++) {
    Serial2.write(command_autorate[i]);
  }
}

void reconnect() {
  while (!client.connected()) {
    String channel = String(WiFi.macAddress());
    if (client.connect(channel.c_str())) {
      // Serial.println("broker connected");
    } else {
      // Serial.print("Failed to connect to MQTT broker, rc=");
      // Serial.print(client.state());
      // Serial.println(" Retrying in 5 seconds...");
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
  json_data += "],\"id\":";
  json_data += String(id_data);
  json_data += ',';
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

void cek_sensor() {
  if (millis() - sensor_wdg > 5000) {
    // Serial.println("sensor not responding, trying to recall");
    ESP.restart();
  }
}

void writeString(uint16_t address, String data) {
  uint16_t len = data.length();
  EEPROM.write((address - 100) / 100, len);
  EEPROM.commit();
  delayMicroseconds(10);
  for (uint16_t i = 0; i < data.length(); i++) {
    EEPROM.write(address + i, data[i]);
    EEPROM.commit();
    delayMicroseconds(10);
  }
}

String readString(uint16_t address) {
  uint16_t len = EEPROM.read((address - 100) / 100);
  String data = "";
  delayMicroseconds(10);
  for (uint16_t i = 0; i < len; i++) {
    data += (char)EEPROM.read(address + i);
    delayMicroseconds(10);
  }
  return data;
}

void parse_serial() {
  if (msg_in.length() > 0) {
    String command = "";
    String cmd_value = "";
    for (byte i = 0; i < msg_in.length(); i++) {
      command += msg_in[i];
      if (msg_in[i] == ':') {
        break;
      }
    }
    byte get_cmd = 0;
    for (byte i = 0; i < (msg_in.length()); i++) {
      if (get_cmd == 1) {
        cmd_value += msg_in[i];
      }
      if (msg_in[i] == ':') {
        get_cmd = 1;
      }
    }
    if (command == ">setdata:") {
      if (cmd_value.length() > 0) {
        // Serial.println(cmd_value);
        // writeString(MSTR0, cmd_value);
        // Serial.print("OK:");
      }
    }
    else if (command == ">reboot:") {
      ESP.restart();
    }
    else if (command == ">getVAR:") {
      String jsonResponse = "{\n";
      jsonResponse += "  \"setup_var\": {\n";
      jsonResponse += "    \"SSID\":\"" + readString(MSTR0) + "\",\n";
      jsonResponse += "    \"password\":\"" + readString(MSTR1) + "\",\n";
      jsonResponse += "    \"broker\":\"" + readString(MSTR2) + "\",\n";
      jsonResponse += "    \"topic\":\"" + readString(MSTR3) + "\"\n";
      jsonResponse += "  }\n";
      jsonResponse += "}\n";

      // Send the JSON response over Serial
      Serial.println(jsonResponse);
    }
    else {
      Serial.println("ERROR");
    }
  }
}

#include "core0_task.h"
