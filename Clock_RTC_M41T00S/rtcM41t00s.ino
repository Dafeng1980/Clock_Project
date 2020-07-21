#include "RTCm41t00slib.h"
#include <U8g2lib.h>
#include <stdint.h>
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define UI_BUFFER_SIZE 64
#define SERIAL_TERMINATOR '\n'
#define TMP112_ADDR  0x49
#define PIN 7

Adafruit_NeoPixel led(6, PIN, NEO_GRB + NEO_KHZ400);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

RTC_M41T00S rtc;

volatile bool pressedButton, n, ledstatus;
volatile uint8_t key;
char daysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thurday", "Friday", "Saturday"};
char dateString[35];
char ui_buffer[UI_BUFFER_SIZE];
uint16_t alarmtime1,alarmtime2;
const uint8_t kBuzzerPin = 15;
const int kButtonPin = 6;

void WakeUp() {
  pressedButton = true;
}

void setup() {
  pinMode(kButtonPin, INPUT);
  pinMode(kBuzzerPin, OUTPUT);
  digitalWrite(kBuzzerPin, LOW);
  Serial.begin(38400);
  led.begin();
  rtc.begin();
  u8g2.begin();
  u8g2.enableUTF8Print(); 
  attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, FALLING);
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//rtc.adjust(DateTime(2020, 6, 18, 18, 40, 30));  
  rtc.setCalibration(0xA6);
  Wire.beginTransmission(TMP112_ADDR);
  Wire.write(0x01);
  Wire.write(0x60);
  Wire.write(0xA0);
  Wire.endTransmission();
  delay(50); 
  adjTimeAlarm();
  rtc.printAllBits();
  EEPROM.get(0x00, alarmtime1);
  led.clear(); 
  led.show();
  key = 0;
  ledstatus = true;
}

void loop() {  
   DateTime now = rtc.now();
  // if (now.hour() == (alarmtime1 >> 8) && now.minute() ==  (alarmtime1 & 0xff) && now.second()<=3) alarmbuzzer();
   switch (key){
    case 0:
    lcdDisplayAll();
    break;
  case 1:
    lcdDisplayA();
    //ledon();
    break;
  case 2:
    lcdDisplayAll();
    key = 0;
    break;
  case 3:
    break;
  default:
   key = 0;
   }
   // lcdDisplayAll();
    //lcdDisplayA();
  //  if(now.minute() == 00 && now.hour() > 6 && now.hour() < 23 && now.second()<3 ) lighton();
    if(now.minute() == 30 && now.hour() > 6 && now.hour() < 23 && now.second()<3 ) lighton();
    if(now.minute() == 00 && now.hour() > 6 && now.hour() < 23 && now.second()<5 ) {
      sound();
      delay(300);
      sound();
      delay(300);
      rainbow(8);
      ledoff();
      delay(1500);
      rainbow(8);
      ledoff();
    }
    
   // DateTime future (now + TimeSpan(7,12,30,6)); 
    delay(1000);
    checkButton();
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
