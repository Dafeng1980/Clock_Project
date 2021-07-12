
void adjTimeAlarm(){
   PrintTime();
  Serial.println("");
  Serial.println(F("T set [T]ime"));
  Serial.println(F("A set [A]larm time"));
  Serial.println("");
  Serial.println(F("Choose a menu item:"));
  Serial.println(F("-------------------"));
  delay(3500);
    if (Serial.available() > 0)
  {
    uint8_t k = Serial.read();
    Serial.read();
    switch (k)
    {
    case 'T':
    case 't':
      SetTime();
      break;
    case 'A':
    case 'a':
     SetAlarmTime();
    default:
      break;
    }
    delay(10);
    Serial.flush();
  }
}

void lcdDisplayAll(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);    
      u8g2.setCursor(0, 10);
      u8g2.print(nowtime.year(), DEC);
      u8g2.print('/');
      u8g2.print(nowtime.month(), DEC);
      u8g2.print('/');
      u8g2.print(nowtime.day(), DEC);
      u8g2.print(' ');
      u8g2.print(daysOfTheWeek[nowtime.dayOfTheWeek()]);
        if(batVal >0)
           {
              u8g2.print(' ');
              u8g2.print(batVal);
              u8g2.print("%");
           }
//        Serial.print("BATTER_VAL(%):=");
//        Serial.println(batVal);
      u8g2.setFont(u8g2_font_10x20_tr);
      u8g2.setCursor(0, 32);
      sprintf(dateString, "%02u:%02u:%02u", nowtime.hour(), nowtime.minute(), nowtime.second());
      u8g2.print(dateString);
      u8g2.print(" ");
      u8g2.print(tempVal, 1);      
      u8g2.sendBuffer();
}

 void lcdDisplayA(){      
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_inr30_mf);
      u8g2.setCursor(0, 32);
      if(st%2) sprintf(dateString, "%02u:%02u", nowtime.hour(), nowtime.minute());     
         else   sprintf(dateString, "%02u %02u", nowtime.hour(), nowtime.minute());
      u8g2.print(dateString);
      u8g2.sendBuffer();
 }

float gettemp(){
  uint8_t data[2];
  float fTemp;
  Wire.beginTransmission(TMP112_ADDR);
  Wire.write(0X00);
  Wire.endTransmission();
  delay(30);
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
         tone(kBuzzerPin, 2900);
          delay(35);
           noTone(kBuzzerPin);
           T2_init();
    }

void halfsound(){
         tone(kBuzzerPin, 2800);
          delay(100);
           noTone(kBuzzerPin);
           T2_init();
    }
    
void alarmbuzzer(){
  for (int i = 0; i < 602; i++) {
    noTone(kBuzzerPin);
    delay(120);
    if (digitalRead(kButtonPin) == 0) {
      delay(5);
      if (digitalRead(kButtonPin) == 0);
      {
//        key = 0;
        break;
      }
    }
//    if (irrecv.decode(&results)){
//      break;
//    }
    tone(kBuzzerPin, 2850);
    delay(80);
}
  noTone(kBuzzerPin);
  T2_init();
}

void PrintTime()
     {
          nowtime = rtc.now();
          sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
          nowtime.year(), nowtime.month(), nowtime.day(), nowtime.hour(),
          nowtime.minute(), nowtime.second());
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

void beep(int bCount,int bDelay)
  {
 // if (mute) return;
  for (int i = 0; i<=bCount; i++)
    {
      digitalWrite(kBuzzerPin,HIGH);
      for(int i2=0; i2<bDelay; i2++)
        {
          __asm__("nop\n\t"); 
        }
          digitalWrite(kBuzzerPin,LOW);
          for(int i2=0; i2<bDelay; i2++)
            {
              __asm__("nop\n\t");
            }
      }
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

void SetAlarmTime(){
  uint8_t x;
  EEPROM.get(0x00, alarmtime1);
  Serial.print(F("alarmtime1 "));
  Serial.print(F("Time "));
  Serial.print(alarmtime1 >> 8);
  Serial.println(alarmtime1 & 0xff);
  Serial.println(" ");
  Serial.println(F("Enter Alarm1 Time format"));
  Serial.println(F("HH:MM  *Sample: 12:34 "));
  x = read_data(); 
  alarmtime1 = (((ui_buffer[0] - '0')*10 + (ui_buffer[1] - '0')) << 8) | ((ui_buffer[3]-'0')*10 + (ui_buffer[4] - '0'));
  Serial.print(F("alarmtime1 "));
  Serial.print(F("Time "));
  Serial.print(alarmtime1 >> 8);
  Serial.println(alarmtime1 & 0xff);
  EEPROM.put(0x00, alarmtime1);
}

void Tmp112_init()
   {
      Wire.beginTransmission(TMP112_ADDR);
      Wire.write(0x01);
      Wire.write(0x60);
      Wire.write(0xA0);
      Wire.endTransmission();
      delay(10); 
   }

   void powerdown()
{
  ADCSRA &= ~(1 << ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_enable();
// sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable(); 
 // Serial.println(F("Sleep_disable"));
  sei();
  ADCSRA |= (1 << ADEN);
}

void setBrightness(){
    Wire.beginTransmission(0x3c);
    Wire.write(0x00);
    Wire.write(0x81);
    Wire.endTransmission();
    Wire.beginTransmission(0x3c);
    Wire.write(0x00);
    Wire.write(0x01);
    Wire.endTransmission();
}

void setLcdOff()
  {
  
    Wire.beginTransmission(0x3c);
    Wire.write(0x00);
    Wire.write(0xAE);
    Wire.endTransmission();
    delay(10);
    Wire.beginTransmission(0x3c);
    Wire.endTransmission();

  }

void setLcdOn()
   {
    int i2cstatus;
    Wire.beginTransmission(0x3c);
    i2cstatus=Wire.endTransmission();
    while(i2cstatus>0)
  {
    Wire.beginTransmission(0x3c);
    i2cstatus=Wire.endTransmission();
  }
    Wire.beginTransmission(0x3c);
    Wire.write(0x00);
    Wire.write(0xAF);
    Wire.endTransmission();
}

void enterPowerDown(){
      setLcdOff();     
      digitalWrite(kLightPin, LOW);      //trun off LED light
      digitalWrite(kPowerSwitch, HIGH);   // Power Switch off
      while (!digitalRead(kButtonPin)) {};
      key = 0;
      st = 0;
      Serial.println(F("Sleep --"));
      delay(200);
      attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, LOW);
      powerdown();
      T2_init();
      sound();
      Serial.println(F("WakeUp--"));
      setLcdOn();
}

void checkButton(){
  if ((st >= 300 && nowtime.hour() >= 0 && nowtime.hour() < 8 ) && !chargerstatus) key = 9;     
  if (digitalRead(kButtonPin) == 0 && buttonstatus) 
    {
      delay(10);
      if (digitalRead(kButtonPin) == 0);
        {           
          key++;
          sound();
          buttonstatus = false;
        }
     }
         cli();
         nShutT = nSysT;
         sei(); 
         while (!digitalRead(kButtonPin)) {
              uint16_t  nTmp = 0;
              cli();
              nTmp = SYSTMMAX + nSysT - nShutT;
              sei();
                if(nTmp >= SYSTMMAX)
                {
                  nTmp = nTmp - SYSTMMAX;
                }
                 if (nTmp >= 1600) {
                  key = 9;
                  break;
                 }
                };
          if (key > 9) key = 0;
          // delay(10);
 }

void hoursbuzz(){
    uint16_t  nTimeTmp = 0;
    if(nowtime.minute() > 30 && buzzstatus) buzzstatus = false;    
    if(nowtime.minute() == 30 && nowtime.hour() > 6 && nowtime.hour() < 23 && buzzstatus )
        {
           halfsound();
           buzzstatus = false;
        }  
    if(nowtime.minute() == 0 && nowtime.hour() > 6 && nowtime.hour() < 23 && !buzzstatus ) 
        {
           nhours = nowtime.twelveHour();  //get the number of hours
           hoursalarm = true;
           buzzstatus = true; 
        }
    if(hoursalarm) hours_alarm(); 
}

void hours_alarm(){
     uint16_t  nTimeTmp = 0;
        cli();
        nTimeTmp = SYSTMMAX + nSysT - nProtT;
        sei();
          if(nTimeTmp >= SYSTMMAX)
          {
            nTimeTmp = nTimeTmp - SYSTMMAX;
          }
          if (nTimeTmp >= 650)   // per 2ms * 650 =1.3s  sound 
          {
            sound();
            nhours--;
            if(nhours == 0) hoursalarm = false;                              
            cli();
            nProtT = nSysT;
            sei(); 
          }  
}

void detectIR(){
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

       switch (results.value) {
         case 0xA32AB931:
         case 0x2F502FD:
              sound();
//              if (lightstatus)
//                  {
//                   digitalWrite(kLightPin, LOW);
//                   lightstatus = false;
//                  } else            
//                        {
//                         analogWrite(kLightPin, lightvalue);
//                         lightstatus = true;
//                        }                       
            break;

         case 0x39D41DC6:
         case 0x2F522DD:
 //     if(lightstatus){
          sound();
//          for (int i = 0; i < 16 ; i++){
//          if(lightvalue == 255)
//          lightvalue = 255;
//          else 
//          lightvalue++;
//          delay(10);
//          analogWrite(kLightPin, lightvalue);
//             }
//         }           
           break;
                     
         case 0xE0984BB6:
         case 0x2F518E7:
          sound();
//      if(lightstatus){
//          for (int i = 0; i < 16 ; i++){
//            if (lightvalue == 0)
//            lightvalue = 0;
//            else
//            lightvalue--;
//            delay(10);
//          analogWrite(kLightPin, lightvalue);
//             }
//         }                  
            break;
            
         case 0x4EA240AE:
            delay(50);
            sound();
//            key++;
//           if(key > 3)
//            key = 0;
//           n = 0;           
            break;

         case 0x4E87E0AB:
              delay(50);
              sound();
            break;

         case 0x371A3C86:
            delay(50);
            sound();
            //DisplayTempHum();
            break;
             
         case 0x143226DB:
            delay(50);
            sound();
           // DisplayAll();
            break;
         }
   irrecv.resume();
   }  
}

void ioInit(){
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kExtPowerPin, INPUT);        //Power input and charger at HIGH
  pinMode(kBuzzerPin, OUTPUT);
  pinMode(kPowerSwitch, OUTPUT);
  pinMode(kLightPin, OUTPUT);
  pinMode(kLedPin, OUTPUT);
  digitalWrite(kPowerSwitch, HIGH);
  digitalWrite(kBuzzerPin, LOW);
  digitalWrite(kPowerSwitch, LOW);
  digitalWrite(kLightPin, LOW);
}
