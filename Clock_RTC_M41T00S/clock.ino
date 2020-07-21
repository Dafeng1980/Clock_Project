
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
      DateTime now = rtc.now();
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_6x10_tr);    
      u8g2.setCursor(0, 10);
      u8g2.print(now.year(), DEC);
      u8g2.print('/');
      u8g2.print(now.month(), DEC);
      u8g2.print('/');
      u8g2.print(now.day(), DEC);
      u8g2.print(' ');
      u8g2.print(daysOfTheWeek[now.dayOfTheWeek()]);
      u8g2.setFont(u8g2_font_10x20_tr);
      u8g2.setCursor(0, 32);
      sprintf(dateString, "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
      u8g2.print(dateString);
      u8g2.print(" ");
      u8g2.print(gettemp(), 1);      
      u8g2.sendBuffer();
}
 void lcdDisplayA(){      
      DateTime now = rtc.now();
      u8g2.clearBuffer();
      u8g2.setFontMode(1);
      u8g2.setFont(u8g2_font_inr30_mf);
      u8g2.setCursor(0, 32);
      if(n=n^1) sprintf(dateString, "%02u:%02u", now.hour(), now.minute());     
         else   sprintf(dateString, "%02u %02u", now.hour(), now.minute());
//      Serial.print("n= ");
//      Serial.println(n);
      u8g2.print(dateString);
      u8g2.sendBuffer();
 }

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

void ledoff(){
    led.clear(); 
    delay(10);
  led.show();
}
void ledon(){
      led.clear(); // Set all pixel colors to 'off'
   led.setBrightness(57);
  for(int i=0; i<6; i++) {

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    led.setPixelColor(i, led.Color(100, 60, 30));
    led.show();   // Send the updated pixel colors to the hardware.
    delay(1); // Pause before next pass through loop
  }
}

void lighton(){
    led.clear(); // Set all pixel colors to 'off'
  //  led.setBrightness(127);
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<6; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    led.setPixelColor(i, led.Color(150, 150, 150));

    led.show();   // Send the updated pixel colors to the hardware.

    delay(1); // Pause before next pass through loop
  }
  delay(1500);
//   for (int i = 800; i>200; i = i- 200){
//            beep(30,i);
//          }
//          delay(500);
          beep(20,600);
            delay(200);
          beep(20,600);
            delay(200);
          beep(20,600);
  led.clear(); 
  led.show();
}

void beep(int bCount,int bDelay){
 // if (mute) return;
  for (int i = 0; i<=bCount; i++){digitalWrite(kBuzzerPin,HIGH);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}digitalWrite(kBuzzerPin,LOW);for(int i2=0; i2<bDelay; i2++){__asm__("nop\n\t");}}
}

void checkButton(){
  if (digitalRead(kButtonPin) == 0) {
    delay(5);
    if (digitalRead(kButtonPin) == 0);
    {
        if (ledstatus){
              ledon();
            ledstatus = false;
              } 
              else            
              {
                ledoff();
                ledstatus = true;
              }
        key++;
    }
  }
//  if (digitalRead(kExtPowerPin) && n > 50) {
//    n = 0;
//  }
//  if (getbatteryval() <= 768) {
//    digitalWrite(kLedPin, HIGH);
//    if (!((n + 1) % 60)) {
//      DispalyShutdown();
//      attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, FALLING);
//      delay(10);
//      powerdown(SLEEP_FOREVER);
//      detachInterrupt(digitalPinToInterrupt(kButtonPin));
//
//    }
//
//  }
//  if (getbatteryval() > 780) {
//    digitalWrite(kLedPin, LOW);
//  }
 }

 void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<led.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / led.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      led.setPixelColor(i, led.gamma32(led.ColorHSV(pixelHue)));
    }
    led.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
