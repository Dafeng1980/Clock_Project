void InitTmp100Hdc1080() {
  Wire.beginTransmission(kTmp100Addr);
  Wire.write(0x01);
  Wire.write(0x31);  // 12 bits Resolution(320ms); in Shutdown Mode
  Wire.endTransmission();
  delay(10);

  Wire.beginTransmission(kHdc1080Addr);
  Wire.write(0x02);
  Wire.write(0x00);   // 14 bits Resolution(6.5ms); Mode = 0; Temperature or Humidity is acquired.
  Wire.write(0x00);
  Wire.endTransmission();
}

void DisplayTime() {
  DateTime now = rtc.now();
  sprintf(dateString, "%02u %02u %02u", now.hour(), now.minute(), now.second());
  display.set(dateString);
  display.setDot(0, true);
  display.setDot(1, true);
  display.setDot(2, true);
  display.setDot(3, true);
  display.setDot(4, true);
  display.setDot(5, true);
  display.show(500);
  display.setDot(0, false);
  display.setDot(1, false);
  display.setDot(2, false);
  display.setDot(3, false);
  display.setDot(4, false);
  display.setDot(5, false);
  display.show(500);
//  if (!((n + 1) % 36)) {
//
//    DisplayTempHum();
//
//  }
}
void DisplayTimeA() {
  uint8_t dow;
  DateTime now = rtc.now();
  sprintf(dateString, "%02u=%02u d%1u", now.hour(), now.minute(), now.dayOfTheWeek());
  display.set(dateString);
  display.show(500);
  sprintf(dateString, "%02u %02u d%1u", now.hour(), now.minute(), now.dayOfTheWeek());
  display.set(dateString);
  display.show(500);
}
void DisplayDate() {
  DateTime now = rtc.now();
  int batteryval = map(getbatteryval(), 750, 980, 0, 100);
  if (batteryval > 85)
    sprintf(dateString, "++ %02u-%02u ", now.month(), now.day());
  else if (batteryval <= 85 && batteryval >70)
    sprintf(dateString, "=+ %02u-%02u ", now.month(), now.day());
  else if (batteryval <= 70 && batteryval >55)
    sprintf(dateString, "_+ %02u-%02u ", now.month(), now.day());
  else if (batteryval <= 55 && batteryval >40)
    sprintf(dateString, " + %02u-%02u ", now.month(), now.day());
  else if (batteryval <= 40 && batteryval > 20)
    sprintf(dateString, " = %02u-%02u ", now.month(), now.day());
  else if (batteryval <= 20)
    sprintf(dateString, " _ %02u-%02u ", now.month(), now.day());
   
  display.set(dateString);
  display.show(2000);
}
void DisplayTemp() {
  int dot = (gettemperature() & 0x00F)*0.625;
  int temp = gettemperature() * 0.0625;
  sprintf(tempString, "%02u#%1uC", temp, dot);
  display.show(tempString, 3000, ALIGN_CENTER);
  String Null = "        ";
  display.set(Null);
  display.show();
}
void DisplayTempHum() {
  byte dot = (gettemperature() & 0x00F)*0.625;
  byte temp = gettemperature() * 0.0625;
  byte hum = gethumidity();
  sprintf(dateString, "%2u#%1u %2uH", temp, dot, hum);
  display.set(dateString);
  display.show(5000);
}
void DisplayAll() {
  // byte dot = (gettemperature() & 0x00F)*0.625;
  byte temp = gettemperature() * 0.0625;
  byte hum = gethumidity();
  int batteryval = map(getbatteryval(), 750, 980, 0, 100);
  DateTime time = rtc.now();
  sprintf(dateString, " %4u-%02u-%02u d%1u %02u=%02u %2uC_%2uH P%2u ", time.year(), time.month(), time.day(), time.dayOfTheWeek(), time.hour(), time.minute(), temp, hum, batteryval);
  String condition = dateString;
  condition = "        " + condition;

  while (condition.length() > 0) {
    display.show(condition, 350, ALIGN_LEFT);
    condition.remove(0, 1);
  }
}
void DisplayInit() {
  for (int i = 1; i < 6; i++) {
    sprintf(tempString, "LOAD");
    display.show(tempString, 500, ALIGN_CENTER);
    String Null = "        ";
    display.set(Null);
    display.show(600);
  }
}
void DispalyShutdown() {
  for (int i = 1; i < 6; i++) {
    sprintf(tempString, "OOOOOO");
    display.show(tempString, 300, ALIGN_CENTER);
    String Null = "        ";
    display.set(Null);
    display.show(300);
  }

}
void DisplayOn() {
  display.show("ON", 3000, ALIGN_CENTER);
}
void DisplayOff() {
  display.show("OFF", 3000, ALIGN_CENTER);
}
//void PrintTime()
//{
//  DateTime time = rtc.now();
//
//  sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
//    time.year(), time.month(), time.day(), time.hour(),
//    time.minute(), time.second());
//  Serial.println(dateString);
//}

//void SetTime()
//{
//  uint8_t x;
//  char date[11], time[8];
//  Serial.println(F("Enter date format"));
//  Serial.println(F("mmm-dd-yyyy hh-mm-ss sample input: Dec 26 2009 12:34:56"));
//
//  while (Serial.available() < 20)
//  {
//  }
//  for (int i = 0; i < 11; i++) {
//    date[i] = Serial.read();
//  }
//  x = Serial.read();
//  for (int i = 0; i < 8; i++) {
//    time[i] = Serial.read();
//  }
//
//  delay(10);
//  Serial.flush();
//  rtc.adjust(DateTime(date, time));
//  delay(10);
//  Serial.println(date);
//  DateTime now = rtc.now();
//  x = now.dayOfWeek();
//  //Serial.print("X=");
//  //Serial.println(x);
//  Wire.beginTransmission(0x68);   //M41T62_ADDRESS
//  Wire.write(0x04);              // SQW Frequency / Day of Week
//  Wire.write(x);
//  Wire.endTransmission();
//
//  Serial.println(F("Set Successful"));
//}
//void SetAlarmTime() {
//  uint8_t x, y;
//  uint8_t sec, min, hour, day, month, mode;
//  DateTime now = rtc.now();
//  month = now.month();
//  day = now.day();
//  sec = 0;
//
//  Serial.println(F("Enter alarm time format (mode is 1 to 6, 4 is per day)"));
//  Serial.println(F("hh:mm mode"));
//
//  while (Serial.available() < 8)
//  {
//  }
//  x = Serial.read(); // hour: tens digit
//  Serial.write(x);
//  y = Serial.read(); // hour: ones digit
//  Serial.write(y);
//  hour = 10 * (x - '0') + (y - '0');
//
//  x = Serial.read(); // discard spacer
//  Serial.write(x);
//  x = Serial.read(); // min: tens digit
//  Serial.write(x);
//  y = Serial.read(); // min: ones digit
//  Serial.write(y);
//  min = 10 * (x - '0') + (y - '0');
//
//  x = Serial.read(); //  discard spacer
//  Serial.write(x);
//  y = Serial.read(); // alarmRepeat mode
//  Serial.write(y);
//  Serial.println(" ");
//  mode = y - '0';
//
//  delay(10);
//  Serial.flush();
//  DateTime alarmTime(2019, month, day, hour, min, sec);
//  rtc.alarmSet(alarmTime);
//  rtc.alarmRepeat(mode);
//  Serial.println(F("Set Successful"));
//  //rtc.alarmRepeat(4);// set alarm repeat mode (once per day)
//}

void SetAlarmTime(){
  uint8_t x;
  uint8_t sec, min, hour, day, month, mode;
  uint16_t year;
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  sec = 0;
//  EEPROM.get(0x00, alarmtime1);
//  Serial.print(F("alarmtime1 "));
//  Serial.print(F("Time "));
//  Serial.print(alarmtime1 >> 8);
//  Serial.println(alarmtime1 & 0xff);
//  Serial.println(" ");
  Serial.println(F("Enter Alarm1 Time format"));
  Serial.println(F("HH:MM  *Sample: 12:34 mode 1/per sec; 2/P min; 3/P hou; 4/P D 5/P M 6/P Y"));
  x = read_data(); 
//  alarmtime1 = (((ui_buffer[0] - '0')*10 + (ui_buffer[1] - '0')) << 8) | ((ui_buffer[3]-'0')*10 + (ui_buffer[4] - '0'));
  hour =(ui_buffer[0] - '0')*10 + (ui_buffer[1] - '0');
  min = (ui_buffer[3]-'0')*10 + (ui_buffer[4] - '0');
  mode = ui_buffer[6] - '0';
  DateTime alarmTime(year, month, day, hour, min, sec);
  rtc.alarmSet(alarmTime);
  rtc.alarmRepeat(mode);
  Serial.println(F("Set Successful"));
}

void SetTime()
{
  uint8_t x,user_command;
  char date[12], times[9];
  Serial.println(F("Enter date format"));
  Serial.println(F("mmm-dd-yyyy  *Sample: Dec 26 2019 "));
  Serial.println(F("mmm//Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec"));
  x = read_data();
  for (int i = 0; i < 12; i++) {
    date[i] = ui_buffer[i];
  }
  Serial.print(F("X: "));
  Serial.println(x, DEC);
  Serial.print(F("Date: "));
  if (x == 0){
    DateTime now = rtc.now();
    char buf1[] = "MMM DD YYYY";
    now.toString(buf1);
    for (int i = 0; i < 12; i++) {
    date[i] = buf1[i];
    }
  }
 // Serial.print(F("Date: "));
  Serial.println(date);
  delay(100);
  Serial.println(F(" "));
  Serial.println(F("Enter time format"));
  Serial.println(F("hh-mm-ss  *Sample: 12:34:56"));
  read_data();
    for (int i = 0; i < 9; i++) {
    times[i] = ui_buffer[i];
  }
  Serial.print(F("Time: "));
  Serial.println(times);
  
  do{
  Serial.print(F("date&time: "));
  Serial.print(date);
  Serial.print(F(" "));
  Serial.println(times); 
  Serial.println(F(" Save To RTC Please Enter: \"Y\" "));
  Serial.println(F(" Quit The RTC Don't Save: \"Q\" "));
  user_command = read_char();
  Serial.println((char)user_command);
  switch (user_command)
  {
    case 'y':
    case 'Y':
    
    Serial.println(F("Save To RTC "));
    rtc.adjust(DateTime(date, times));
    delay(10);
    Serial.println(date);
//    DateTime now = rtc.now();
    
//    x = now.dayOfTheWeek();
//    //Serial.print("X=");
//    //Serial.println(x);
//    Wire.beginTransmission(0x68);   //M41T62_ADDRESS
//    Wire.write(0x04);              // SQW Frequency / Day of Week
//    Wire.write(x);
//    Wire.endTransmission();
    Serial.println(F("Set Successful"));    
    break;
    
    case 'q':
    case 'Q':   
    Serial.println(F("Quit & Don't Save RTC"));  
    break;
    
    default:
       if (user_command != 'y' || user_command != 'Y' || user_command != 'Q' || user_command != 'q')
        Serial.println(F("Invalid Selection !!"));
     break;
  }
  }
    while (user_command != 'y' && user_command != 'Y' && user_command != 'Q' && user_command != 'q');
    Serial.println(F(" Exit Set Time"));
}


void PrintTime()
     {
          DateTime time = rtc.now();

            sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
           time.year(), time.month(), time.day(), time.hour(),
           time.minute(), time.second());
           Serial.println(dateString);
      }

int getbatteryval() {
  int sum = 0;
  for (int i = 0; i < 3; i++) {
    sum += analogRead(kBatteryPin);
  }
  return sum = sum / 3;
}

int gettemperature() {
  Wire.beginTransmission(kTmp100Addr);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(kTmp100Addr, 1);
  uint8_t cros = Wire.read();
  bitWrite(cros, 6, 1);               //One-shot Temperature

  Wire.beginTransmission(kTmp100Addr);
  Wire.write(0x01);
  Wire.write(cros);
  Wire.endTransmission();
  delay(330);

  Wire.beginTransmission(kTmp100Addr);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(kTmp100Addr, 2);
  byte msb = Wire.read();
  byte lsb = Wire.read();
  int temp = ((msb << 8) | lsb) >> 4;
  //int temp = ((msb * 256) + lsb) / 16;
  if (temp >= 2048) {
    temp -= 4096;
  }
  return temp;
}
double gethumidity() {
  Wire.beginTransmission(kHdc1080Addr);
  Wire.write(0x01);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(kHdc1080Addr, 2);
  byte msb = Wire.read();
  byte lsb = Wire.read();
  uint16_t hum = (msb << 8) | lsb;
  //Serial.print("Hum:=");
  //Serial.println(hum);
  double fhum = (hum / pow(2, 16)) * 100.0;
  return  fhum;
}

void sound(){
         tone(kSpeakerPin, 3530);
          delay(50);
           noTone(kSpeakerPin);
    }
void PlayMusic() {
  irrecv.resume();
  for (int i = 0; i < 50; i++) {
    note(musicreadeeprom(2 * i), EIGHTHNOTE);
  }
  rest(EIGHTHNOTE);
  /////// KEEP ALL CODE BELOW UNCHANGED, CHANGE VARS ABOVE ////////
  for (int i = 0; i < 602; i++) {
    noTone(kSpeakerPin);
    delay(120);
    if (digitalRead(kButtonPin) == 0) {
      delay(5);
      if (digitalRead(kButtonPin) == 0);
      {
        key = 0;
        break;
      }
    }
    if (irrecv.decode(&results)){
      break;
    }
    tone(kSpeakerPin, 2050);
    delay(80);
  }
  noTone(kSpeakerPin);
}
void spacedNote(int frequencyInHertz, int noteLength)
{
  tone(kSpeakerPin, frequencyInHertz);
  float delayTime = (1000 / kTempo) * (60 / 4) * noteLength;
  delay(delayTime - 50);
  noTone(kSpeakerPin);
  delay(50);
}
void note(int frequencyInHertz, int noteLength)  //Code to take care of the note
{
  tone(kSpeakerPin, frequencyInHertz);
  float delayTime = (1000 / kTempo) * (60 / 4) * noteLength;
  delay(delayTime);
}
void rest(int restLength)
{
  noTone(kSpeakerPin);
  float delayTime = (1000 / kTempo) * (60 / 4) * restLength;
  delay(delayTime);
}

int musicreadeeprom(uint16_t address) {
  int data;
  Wire.beginTransmission(kM24lc128Addr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.endTransmission(I2C_NOSTOP);
  Wire.requestFrom(kM24lc128Addr, 2);
  uint8_t dataL = Wire.read();
  uint8_t dataH = Wire.read();
  data = dataH * 256 + dataL;
  return data;
}
void m24lc128writebyte(uint16_t address, uint8_t  data)
{
  Wire.beginTransmission(kM24lc128Addr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.write(data);
  Wire.endTransmission();
  delay(6);
}
void m24lc128writebytes(uint16_t address, uint8_t count, uint8_t * dest)
{
  if (count > 64) {
    count = 64;
    //  Serial.print("Page count cannot be more than 64 bytes!");
  }

  Wire.beginTransmission(kM24lc128Addr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  for (uint8_t i = 0; i < count; i++) {
    Wire.write(dest[i]);
  }
  Wire.endTransmission();
}
uint8_t m24lc128readbyte(uint16_t address)
{
  uint8_t data;
  Wire.beginTransmission(kM24lc128Addr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.endTransmission(I2C_NOSTOP);
  Wire.requestFrom(kM24lc128Addr, 1);
  data = Wire.read();
  return data;
}
void m24lc128readbytes(uint16_t address, int count, uint8_t * dest)
{
  Wire.beginTransmission(kM24lc128Addr);
  Wire.write((int)(address >> 8));
  Wire.write((int)(address & 0xFF));
  Wire.endTransmission(I2C_NOSTOP);
  uint8_t i = 0;
  Wire.requestFrom(kM24lc128Addr, count);
  while (Wire.available()) {
    dest[i++] = Wire.read();
  }
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



void powerdown(byte period)
{
  ADCSRA &= ~(1 << ADEN);

  if (period != SLEEP_FOREVER)
  {
    wdt_enable(period);
    WDTCSR |= (1 << WDIE);
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();
  ADCSRA |= (1 << ADEN);
}
