
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
      u8g2.setFont(u8g2_font_6x12_tr);    
      u8g2.setCursor(0, 12);
      u8g2.printf("%02u/%02u/%02u %s ",nowtime.year(),nowtime.month(),nowtime.day(),daysOfTheWeek[nowtime.dayOfTheWeek()]);
      if(batVal >0)
        {
          u8g2.print(batVal);
          u8g2.print("%");
        }
//        Serial.printf("BATTER_VAL(%):=");
//        Serial.println(batVal);
      u8g2.setFont(u8g2_font_10x20_tr);
      u8g2.setCursor(0, 32);
      u8g2.printf("%02u:%02u:%02u",nowtime.hour(),nowtime.minute(),nowtime.second());
      u8g2.setFont(u8g2_font_8x13_tr);
      u8g2.print(" ");
      u8g2.print(tempVal, 1);      
      u8g2.sendBuffer();
}

 void lcdDisplayA(){
      goneSeconds = millis() / 1000;       
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x12_tr);    
      u8g2.setCursor(98, 12);
      u8g2.printf("%s",daysOfTheWeek[nowtime.dayOfTheWeek()]);
      u8g2.setFont(u8g_font_courB24n);
      u8g2.setCursor(0, 32);
      if(goneSeconds % 2) sprintf(dateString, "%02u:%02u", nowtime.hour(), nowtime.minute());     
         else  sprintf(dateString, "%02u %02u", nowtime.hour(), nowtime.minute());
      u8g2.print(dateString);
      u8g2.sendBuffer();
      
 }

void lcdDisplayTemp(){
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_9x15_tr);    
      u8g2.setCursor(92, 15);
      if(batVal >0)
        {
          u8g2.print(batVal);
          u8g2.print("%");
        }
      u8g2.setFont(u8g_font_courB24n);
      u8g2.setCursor(0, 32);
      u8g2.print(tempVal, 1);
      u8g2.sendBuffer(); 
}

float gettemp(){
  uint8_t data[2];
  byte error;
  float fTemp;
  Wire.beginTransmission(TMP112_ADDR);
  Wire.write(0X00);
  error = Wire.endTransmission();
  delay(10);
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
          delay(30);
           noTone(kBuzzerPin);
    }

void halfsound(){
         tone(kBuzzerPin, 2800);
          delay(85);
           noTone(kBuzzerPin);
    }
    
void alarmbuzzer(){
  for (int i = 0; i < 602; i++) {
    noTone(kBuzzerPin);
    delay(120);
    if (digitalRead(kButtonPin) == 0) {
      delay(5);
      if (digitalRead(kButtonPin) == 0);
      {
//      key = 0;
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

void tmp112_Init()
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
  sleep_disable();    //after run WakeUp();
  ADCSRA |= (1 << ADEN);
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
    i2cstatus = Wire.endTransmission();
    delay(20);
    while(i2cstatus>0)
  {
    Wire.beginTransmission(0x3c);
    i2cstatus=Wire.endTransmission();
    delay(20);
  }
    Wire.beginTransmission(0x3c);
    Wire.write(0x00);
    Wire.write(0xAF);
    Wire.endTransmission();
}

void enterPowerDown(){
      setLcdOff();     
      digitalWrite(kLightPin, LOW);      //trun off LED light
      digitalWrite(kPowerSwitch, HIGH);  // Power Switch off
      while (!digitalRead(kButtonPin)) {};
      key = 0;
      delay(20);
      attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, LOW);
      powerdown();
      setLcdOn();    
}

void checkButton(){
  goneMinutes = (millis() - sleepmillis) / 60000;
  if ((nowtime.hour() >= 8) || chargerstatus) sleepmillis =  millis(); 
  else if(goneMinutes >= 5) key = 9;
  if (digitalRead(kButtonPin) == 0 && buttonstatus) 
    {
      delay(10);
      if (digitalRead(kButtonPin) == 0);
        {   
          sound();        
          key++;          
//        Serial.printf("key 1 = %02u \n", key);
          buttonstatus = false;
        }
     }
    buttonmillis = millis(); 
  while (!digitalRead(kButtonPin)) {
       if ((millis() - buttonmillis) >= 3500) {
                  key = 9;
                  break;
               }
          }
              
  if (batterylow) {                                      // Bettery Raw data less then 450, the unit will be off.    
    if (((millis() - delaymillis) / 1000) >= 20 ) key = 9;                
  }
  else delaymillis = millis();
  
  if (key > 9) key = 0;
 }

void lightOn(){
  if(!lightstatus){
  analogWrite(kLightPin, Dimmers[dim_data]);
  lightstatus = true;
  }
}

void lightOff(){
  if(lightstatus){
  digitalWrite(kLightPin, LOW);
  lightstatus = false;
  }
}

void hoursbuzz(){
    if(nowtime.minute() > 30 && buzzstatus) buzzstatus = false;    
    if(nowtime.minute() == 30 && nowtime.hour() > 6 && nowtime.hour() < 23 && buzzstatus )
        {
           halfsound();
           buzzstatus = false;
        }  
    if(nowtime.minute() == 0 && nowtime.hour() > 6 && nowtime.hour() < 23 && !buzzstatus ) 
        {
           nhours = nowtime.twelveHour();  //get the number of hours
           sound();
           nhours--;
           alarmtmillis = millis();
           hoursalarm = true;
           buzzstatus = true; 
        }
    if(hoursalarm) {
          if ((millis() - alarmtmillis) >= 1300)  // per 1300ms  sound 
          {
            alarmtmillis = millis();
            sound();
            nhours--; 
            Serial.printf("nhours  = %02u \n", nhours);                                        
          }
        if(nhours == 0) hoursalarm = false; 
    }
}

void detectIR(){
  if (irrecv.decode(&results)) {
     Serial.println(results.value, HEX);
     
       switch (results.value) { 
              
         case 0xA70:                          //Turn on  LED light, via IR to control by smart Phone, or XiaoMi AI speaker(voice control).
         case 0x71CBB48C:
              sound();
              irkey = 0;
              if (lightstatus)
                  {
                   digitalWrite(kLightPin, LOW);
                   lightstatus = false;
                  } 
              else            
                  {
                    if (dim_data !=15)
                          analogWrite(kLightPin, Dimmers[dim_data]);
                    else 
                          analogWrite(kLightPin, 255);
                   lightstatus = true;
                   }                                             
            break;

         case 0xCD0:
         case 0xE05CFF73:
          sound();
          irkey = 1;                   
           break;
                     
         case 0x2D0:
         case 0x51BB112E:
          sound();
          irkey = 2;               
            break;
            
         case 0x4EA240AE:
            delay(50);
            sound();       
            break;

         case 0x4E87E0AB:
              delay(50);
              sound();
            break;

         case 0x371A3C86:
            delay(50);
            sound();
            break;
             
         case 0x143226DB:
            delay(50);
            sound();
            break;
         }
   irrecv.resume();
   }  
}

void lightAdjust()
   {
      if (!chargerstatus && batVal < 50){               //External Power or Battery more then 50%, the LED can be light.
           digitalWrite(kLightPin, LOW);
           lightstatus = false;
      }
      if (lightstatus)
         {
            if (irkey > 0 && irkey < 3)
              {
                 if (irkey == 2)
                  {
                    if (dim_data > 0) dim_data--;
                   }
                 else if (irkey == 1)
                    {
                    if (dim_data < 15)  dim_data++;
                    }
                analogWrite(kLightPin, Dimmers[dim_data]);
//                Serial.printf("Dimmers[dim_data] = %02u \n", Dimmers[dim_data]);
                irkey = 0;
              }
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

//void beep1(){       //short beep on the buzzer
//  if (beepEnable) {
//    for (uint8_t i=0; i<200; i++) {
//      digitalWrite(kBuzzerPin, HIGH);
//      delayMicroseconds(130);
//      digitalWrite(kBuzzerPin, LOW);
//      delayMicroseconds(130);
//    }
//  }
//}

//void beep(int bCount,int bDelay)
//  {
// // if (mute) return;
//  for (int i = 0; i<=bCount; i++)
//    {
//      digitalWrite(kBuzzerPin,HIGH);
//      for(int i2=0; i2<bDelay; i2++)
//        {
//          __asm__("nop\n\t"); 
//        }
//          digitalWrite(kBuzzerPin,LOW);
//          for(int i2=0; i2<bDelay; i2++)
//            {
//              __asm__("nop\n\t");
//            }
//      }
//   }

void getEEPROM() {
  uint16_t identifier = (EEPROM.read(0) << 8) | EEPROM.read(1);
  if (identifier == EEPROM_IDENT) {
     dim_data   =  EEPROM.read(2);
     lightflag  =  EEPROM.read(3);
     alarmtime1 = (EEPROM.read(4) << 8) | EEPROM.read(5);
     alarmtime2 = (EEPROM.read(6) << 8) | EEPROM.read(7);
  }
  else {
    EEPROM.update(0, EEPROM_IDENT >> 8); EEPROM.update(1, EEPROM_IDENT & 0xFF);
    updateEEPROM();
  }
}

void updateEEPROM() {
  EEPROM.update( 2, dim_data);
  EEPROM.update( 3, lightflag);
  EEPROM.update( 4, alarmtime1 >> 8);
  EEPROM.update( 5, alarmtime1 & 0xFF);
  EEPROM.update( 6, alarmtime2 >> 8);
  EEPROM.update( 7, alarmtime2 & 0xFF);
}


void i2cdetects(uint8_t first, uint8_t last) {
  uint8_t i, address, error;
  char buff[10];
  Serial.print("   ");    // table header
  for (i = 0; i < 16; i++) {
    Serial.printf("%3x", i);
  }
  // table body
  for (address = 0; address <= 127; address++) {    // addresses 0x00 through 0x77
    if (address % 16 == 0) {
      Serial.printf("\n%#02x:", address & 0xF0);
    }
    if (address >= first && address <= last) {
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
      delay(5);
      if (error == 0) {                     // device found       
        Serial.printf(" %02x", address);
      } else if (error == 4) {               // other error       
        Serial.print(" XX");
      } else {                              // error = 2: received NACK on transmit of address                        
        Serial.print(" --");               // error = 3: received NACK on transmit of data
      }
    } else {      
      Serial.print("   ");               // address not scanned 
    }
  }
  Serial.println("\n");
}
