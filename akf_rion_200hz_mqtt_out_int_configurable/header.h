#include <PubSubClient.h>
#include <WiFi.h>
#include <EEPROM.h>

#include <esp_task_wdt.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


// user typedef
#define RXD2 16
#define TXD2 17
#define EEPROM_SIZE 1024
#define MSTR0 100
#define MSTR1 200
#define MSTR2 300
#define MSTR3 400
#define MSTR4 500

#define LEDSTATUSPIN 4
#define VSENSE_PIN 33

// global objects 
TaskHandle_t Task2;
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

const int DATA_SIZE = 512;

int x_values[2][DATA_SIZE];
int y_values[2][DATA_SIZE];
int z_values[2][DATA_SIZE];
int data_count = 0;
int last_ts = 0;

unsigned long start_time = 0;
unsigned long wlan_timer = 0;
unsigned long sensor_wdg = 0;
unsigned long ts_timer = 0;
unsigned long id_data = 0;
unsigned long no_serial_in_wdg = 0;

float v_batt = 0.0;

// Function declarations

//RTOS tasks
void mqtt_sender(void *arguments);
void serial_handler(void *arguments);
void led_status(void *arguments);
void sensor_reader( void * pvParameters );

// WiFi and MQTT-related functions
void reconnect();
void publish_buffer(byte buffer_loc);

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

// EEPROM data read/write functions
void writeString(uint16_t address, String data);
String readString(uint16_t address);

// Serial communication and JSON parsing functions
void parse_serial();
void parseJsonData(const String &jsonData);

// include functions that need global variables
#include "jsonparser.h"
#include "serialparser.h"
#include "eepromstorage.h"
#include "sensorutils.h"
#include "mqttutils.h"
#include "rtostasks.h"
