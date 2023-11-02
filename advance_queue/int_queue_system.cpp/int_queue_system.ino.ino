const int maxArraySize = 1000;
int dataArray[maxArraySize];
int arraySize = 0;
unsigned long counter = 0;
int send_duration = 1000;

void sendDataTask(void *parameter) {
  while (1) {
    if (arraySize > 0) {
      sendData(dataArray[0]);
      removeDataFromArray();
    }
    vTaskDelay(send_duration / portTICK_PERIOD_MS);
  }
}

void readSensorTask(void *parameter) {
  while (1) {
    int sensorData = readSensor();
    if (sensorData != -1) {
      addDataToArray(sensorData);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

int sendData(int data) {
  Serial.print(data);
  Serial.print(",");
  Serial.println(arraySize);
  return 1;
}

int readSensor() {
  // Read sensor data here; return -1 if no data is available
  counter++;
  return counter;
}

void addDataToArray(int data) {
  if (arraySize < maxArraySize) {
    dataArray[arraySize] = data;
    arraySize++;
  }
}

void removeDataFromArray() {
  if (arraySize > 0) {
    // Shift elements to the left to remove the first element
    for (int i = 0; i < arraySize - 1; i++) {
      dataArray[i] = dataArray[i + 1];
    }
    arraySize--;
  }
}


void setup() {
  delay(5000);
  pinMode(0, INPUT);
  Serial.begin(115200);
  xTaskCreatePinnedToCore(
    readSensorTask,
    "reader",
    1024,
    NULL,
    1,
    NULL,
    0);
  xTaskCreatePinnedToCore(
    sendDataTask,
    "sender",
    1024,
    NULL,
    1,
    NULL,
    1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(0) == LOW)send_duration = 10;
  else send_duration = 1000;
  vTaskDelay(10 / portTICK_PERIOD_MS);
  Serial.println("[APP] Free memory: " + String(esp_get_free_heap_size()/1024) + " KBytes");
}
