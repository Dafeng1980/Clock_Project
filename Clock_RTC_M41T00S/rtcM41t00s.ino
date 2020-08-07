
#include "RTCm41t00slib.h"
#include <U8g2lib.h>
#include <stdint.h>
#include <EEPROM.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define UI_BUFFER_SIZE 64
#define SERIAL_TERMINATOR '\n'
#define TMP112_ADDR  0x49
#define PIN 20
#define IRPIN 21

Adafruit_NeoPixel led(6, PIN, NEO_GRB + NEO_KHZ400);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

RTC_M41T00S rtc;
IRrecv irrecv(IRPIN);
decode_results results;

volatile bool pressedButton, n, ledstatus;
volatile uint8_t key;
char daysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thurday", "Friday", "Saturday"};
char dateString[35];
char ui_buffer[UI_BUFFER_SIZE];
uint16_t alarmtime1,alarmtime2;
const uint8_t kBuzzerPin = 15;
const uint8_t kPowerSwitch = 10;
const uint8_t kButtonPin = 7;
const uint8_t kExtPowerPin = 22;
const int kBatteryPin = A0;
uint16_t nSysT = 0;

void WakeUp() {
  pressedButton = true;
}

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kExtPowerPin, INPUT);
  pinMode(kBuzzerPin, OUTPUT);
  pinMode(kPowerSwitch, OUTPUT);
   digitalWrite(kPowerSwitch, HIGH);
   digitalWrite(kBuzzerPin, LOW);
  analogReference(INTERNAL2V56);
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
  key = 0;
  ledstatus = true;
  irrecv.enableIRIn();
  digitalWrite(kPowerSwitch, LOW);
  delay(50);
  ledoff();
}

void loop() {  
   DateTime now = rtc.now();
  byte extp = digitalRead(kExtPowerPin);
  Serial.print(F("kExtPowerPin:="));
  Serial.println(extp);
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
    if(now.minute() == 30 && now.hour() > 6 && now.hour() < 23 && now.second()<2 ) lighton();
    if(now.minute() == 00 && now.hour() > 6 && now.hour() < 23 && now.second()<5 ) 
      {
        sound();
        delay(300);
        sound();
        delay(300);
        rainbow(10);
        ledoff();
      }
      
      checkButton();
      detectIR();
   // DateTime future (now + TimeSpan(7,12,30,6)); 
    delay(1000);
}

void checkButton(){
  if (digitalRead(kButtonPin) == 0) {
    delay(5);
    if (digitalRead(kButtonPin) == 0);
    {
        if (ledstatus)
           {
              ledon();
              ledstatus = false;
           } 
        else            
            {
                ledoff();
                ledstatus = true;
            }            
        key++;
        sound();
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

 void T2_init(){
 // OCR = 0x00;
  TCNT2 =0x00;
  TCCR2 = 0x03;     //0x03 clkI/O/64 per 2ms! 0x04 clkI/O/256  per 8ms!
  TIMSK |= (1 << TOIE2); 
}

ISR(TIMER2_OVF_vect)
{
      nSysT++;
      if(nSysT >= 60000)  // after 2min reset 
    {
      nSysT = 0;
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
