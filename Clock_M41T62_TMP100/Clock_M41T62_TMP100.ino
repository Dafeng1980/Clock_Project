// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
  Name:       newClock_M41T62.ino
  Created:  2019/6/29 9:23:56
  Author:     DAFENG\Dafeng
*/

#include <Wire.h>
#include <avr/wdt.h>
#include "M41T62CLOCK.h"
#include <ShiftDisplay.h>
#include <IRremote.h>


#define SIXTEENTHNOTE 1
#define EIGHTHNOTE 2
#define DOTTEDEIGHTNOTE 3
#define QUARTERNOTE 4
#define DOTTEDQUARTERNOTE 6
#define HALFNOTE 8
#define DOTTEDHALFNOTE 12
#define WHOLENOTE 16
#define UI_BUFFER_SIZE 64

#define SLEEP_FOREVER 10
#define SLEEP_8S 9
#define SLEEP_4S 8
#define SLEEP_2S 7
#define SLEEP_1S 6
#define I2C_NOSTOP 0
#define IRPIN 3

RTC_M41T62 rtc;
ShiftDisplay display(14, 15, 13, COMMON_ANODE, 8);
IRrecv irrecv(IRPIN);
decode_results results;

volatile bool pressedButton, alarmstatus,lightstatus;
volatile int key;

const int kTempo = 120;
const int kHdc1080Addr = 0x40;
const int kTmp100Addr = 0x4A;
const int kM24lc128Addr = 0x50;

const int kLedPin = 0;
const int kExtPowerPin = 1;
const int kButtonPin = 2;
// const int kIrPin = 3;
const int kLightPin = 4;
const int kSpeakerPin = 12;
const int kBatteryPin = A7;

char ui_buffer[UI_BUFFER_SIZE];
char dateString[35];
char tempString[5];
int n, st;
uint8_t lightvalue;

void WakeUp() {
  pressedButton = true;
}

void setup()
{
  pinMode(kButtonPin, INPUT);
  pinMode(kExtPowerPin, INPUT);
  pinMode(kSpeakerPin, OUTPUT);
  pinMode(kLedPin, OUTPUT);
  pinMode(kLightPin, OUTPUT);
  digitalWrite(kLedPin, LOW);
  digitalWrite(kLightPin, LOW);
  lightstatus = false;
  lightvalue = 127;
  // attachInterrupt(digitalPinToInterrupt(kButtonPin), buttonPressInterrupt, FALLING);
  analogReference(INTERNAL1V1);
  pressedButton = false;
  alarmstatus = true;
  Serial.begin(38400);
  rtc.begin();
  rtc.checkFlags();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  InitTmp100Hdc1080();
 // PrintTime();
  PrintTime();
  Serial.println("");
  Serial.println(F("T set [T]ime"));
  Serial.println(F("A set [A]larm time"));
  Serial.println("");
  Serial.println(F("Choose a menu item:"));
  Serial.println(F("-------------------"));
  DisplayInit();
  int val = getbatteryval();
  Serial.print("BATTER_VAL:=");
  Serial.println(val);
  val = map(val, 750, 980, 0, 100);
  Serial.print("BATTER_VAL(%):=");
  Serial.println(val);

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
  // rtc.alarmRepeat(4);  // set alarm repeat mode (once per Day)
  Serial.print(F("Repeat Mode Set: "));
  Serial.println(rtc.alarmRepeat());
  rtc.alarmEnable(1);
  rtc.printAllBits();
  DisplayAll();
  if (alarmstatus)
    DisplayOn();
  else
  {
    DisplayOff();
  }
  key = 0;
  n = 0;
  st = 0;
  byte extp = digitalRead(kExtPowerPin);
  Serial.print(F("kExtPowerPin:="));
  Serial.println(extp);
  irrecv.enableIRIn();
}

void loop()
{
  if (pressedButton) {
    detachInterrupt(digitalPinToInterrupt(kButtonPin));
    pressedButton = false;
    if (alarmstatus)
      DisplayOn();
    else
    {
      DisplayOff();
    }
    DisplayAll();
    key = 0;
    n = 0;
    st = 0;
  }
  switch (key)
  {
  case 0:
    DisplayTime();
    break;
  case 1:
    DisplayTimeA();
    st = 0;
    break;
  case 2:
    DisplayDate();
    if (st > 3) {
      key = 0;
    }
    st++;
    break;
  case 3:
    DisplayTempHum();
    st = 0;
    break;
  case 4:
    DisplayAll();
    if (alarmstatus)
      DisplayOn();
    else
    {
      DisplayOff();
    }
    if (st > 0) {
      key = 0;
    }
    st++;
    break;

  case 5:
    DispalyShutdown();
    attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, FALLING);
    digitalWrite(kLightPin, LOW);
    lightstatus = false;
    delay(10);
    powerdown(SLEEP_FOREVER);
    detachInterrupt(digitalPinToInterrupt(kButtonPin));
  default:
    key = 0;
    if (alarmstatus) {
      alarmstatus = false;
    }
    else
    {
      alarmstatus = true;
    }
    break;
  }
  if (n == 290)
  {
    DispalyShutdown();
    attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, FALLING);
    digitalWrite(kLightPin, LOW);
    lightstatus = false;
    delay(10);
    powerdown(SLEEP_FOREVER);
    detachInterrupt(digitalPinToInterrupt(kButtonPin));
  }
  n++;
  detectIR();
  checkButton();
  //Serial.print("n:=");
  //Serial.println(n);
  //Serial.print("key:=");
  //Serial.println(key);
  //Serial.print("alarmstatus:=");
  //Serial.println(alarmstatus);
  //Serial.print("weekend:=");
  //Serial.println(weekend);
}


void detectIR(){
  if (irrecv.decode(&results)) {
   // Serial.println(results.value, HEX);

       switch (results.value) {
         case 0xA32AB931:
         case 0x2F502FD:
              sound();
              if (lightstatus)
                  {
                   digitalWrite(kLightPin, LOW);
                   lightstatus = false;
                  } else            
                        {
                         analogWrite(kLightPin, lightvalue);
                         lightstatus = true;
                        }                       
            break;

         case 0x39D41DC6:
         case 0x2F522DD:
			if(lightstatus){
          sound();
          for (int i = 0; i < 16 ; i++){
          if(lightvalue == 255)
          lightvalue = 255;
          else 
          lightvalue++;
          delay(10);
          analogWrite(kLightPin, lightvalue);
             }
         }           
           break;
                     
         case 0xE0984BB6:
         case 0x2F518E7:
          sound();
			if(lightstatus){
          for (int i = 0; i < 16 ; i++){
            if (lightvalue == 0)
            lightvalue = 0;
            else
            lightvalue--;
            delay(10);
          analogWrite(kLightPin, lightvalue);
             }
         }                  
            break;
            
         case 0x4EA240AE:
            delay(50);
            sound();
            key++;
           if(key > 3)
            key = 0;
           n = 0;           
            break;

         case 0x4E87E0AB:
              delay(50);
              sound();
			        alarmstatus = !alarmstatus;
				      if (alarmstatus)
					      DisplayOn();
				      else
      					{
      					DisplayOff();
      					} 
            break;

         case 0x371A3C86:
            delay(50);
            sound();
            DisplayTempHum();
            break;
             
         case 0x143226DB:
            delay(50);
            sound();
            DisplayAll();
            break;
         }
   irrecv.resume();
   }
  
}
void checkButton() {
  if (digitalRead(kButtonPin) == 0) {
    delay(5);
    if (digitalRead(kButtonPin) == 0);
    {
      if (rtc.checkFlags() && alarmstatus) {
        byte week;
        DateTime now = rtc.now();
        week = now.dayOfTheWeek();
        //Serial.print("week:=");
        //Serial.println(week);
        if (week != 0 && week != 6)
          PlayMusic();
          key = 0;
      }
      else
        key++;
      n = 0;
    }
    sound();
  }
  if (digitalRead(kExtPowerPin) && n > 50) {
    n = 0;
  }
  if (getbatteryval() <= 768) {
    digitalWrite(kLedPin, HIGH);
    if (!((n + 1) % 60)) {
      DispalyShutdown();
      attachInterrupt(digitalPinToInterrupt(kButtonPin), WakeUp, FALLING);
      delay(10);
      powerdown(SLEEP_FOREVER);
      detachInterrupt(digitalPinToInterrupt(kButtonPin));

    }

  }
  if (getbatteryval() > 780) {
    digitalWrite(kLedPin, LOW);
  }
}
