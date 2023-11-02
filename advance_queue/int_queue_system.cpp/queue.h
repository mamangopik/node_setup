void addDataToArray(const char* jsonString) {
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


if (buffer_0_ready == 1) {
  addDataToArray(publish_buffer(0));
  buffer_0_ready = 0;
}
if (buffer_1_ready == 1) {
  addDataToArray(publish_buffer(1));
  buffer_1_ready = 0;
}