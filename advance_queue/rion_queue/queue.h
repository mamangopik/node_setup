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