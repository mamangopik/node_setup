void Task2code( void * pvParameters ) {
  last_ts = 0;
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  sensor_wdg = millis();
  set_autorate();
  for (;;) {
    if (WiFi.status() == WL_DISCONNECTED) {
      ESP.restart();
    }
    cek_sensor();
    while (Serial2.available() > 0) {
      cek_sensor();
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
        process_command();
      }
    }
  }
}
