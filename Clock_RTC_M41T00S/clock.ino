
float gettemp(){
  uint8_t data[2];
  float fTemp;
  Wire.beginTransmission(TMP112_ADDR);
  Wire.write(0X00);
  Wire.endTransmission();
  delay(50);
  Wire.requestFrom(TMP112_ADDR, 2);
  if (Wire.available() == 2) {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  int temp1 = ((data[0] * 256) + data[1]) / 16;      // (msb << 8 |lsb) >> 4;
  if (temp1 >= 2048) {
    temp1 -= 4096;
  }
  return fTemp = temp1 * 0.0625;
}

void sound(){
         tone(kBuzzerPin, 2100);
          delay(100);
           noTone(kBuzzerPin);
    }
void alarmbuzzer(){
  for (int i = 0; i < 602; i++) {
    noTone(kBuzzerPin);
    delay(120);
//    if (digitalRead(kButtonPin) == 0) {
//      delay(5);
//      if (digitalRead(kButtonPin) == 0);
//      {
//        key = 0;
//        break;
//      }
//    }
//    if (irrecv.decode(&results)){
//      break;
//    }
    tone(kBuzzerPin, 2050);
    delay(80);
}
  noTone(kBuzzerPin);
}
    void PrintTime()
{
  DateTime time = rtc.now();

  sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
    time.year(), time.month(), time.day(), time.hour(),
    time.minute(), time.second());
  Serial.println(dateString);
}

uint8_t read_data()
{
  uint8_t index = 0; //index to hold current location in ui_buffer
  int c; // single character used to store incoming keystrokes
  while (index < UI_BUFFER_SIZE-1)
  {
    c = Serial.read(); //read one character
    if (((char) c == '\r') || ((char) c == '\n')) break; // if carriage return or linefeed, stop and return data
    if ( ((char) c == '\x7F') || ((char) c == '\x08') )   // remove previous character (decrement index) if Backspace/Delete key pressed      index--;
    {
      if (index > 0) index--;
    }
    else if (c >= 0)
    {
      ui_buffer[index++]=(char) c; // put character into ui_buffer
    }
  }
  ui_buffer[index]='\0';  // terminate string with NULL

  if ((char) c == '\r')    // if the last character was a carriage return, also clear linefeed if it is next character
  {
    delay(10);  // allow 10ms for linefeed to appear on serial pins
    if (Serial.peek() == '\n') Serial.read(); // if linefeed appears, read it and throw it away
  }

  return index; // return number of characters, not including null terminator
}

int8_t read_char()
{
  read_data();
  return(ui_buffer[0]);
}
