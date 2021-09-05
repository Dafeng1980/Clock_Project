/*   Nano Meega board @ 8Mhz , 

*/
#include <avr/sleep.h> 
#include "RTCm41t00slib.h"
#include <U8g2lib.h>
#include <stdint.h>
#include <EEPROM.h>
#include <IRremote.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define UI_BUFFER_SIZE 64
#define SERIAL_TERMINATOR '\n'
#define TMP112_ADDR  0x49
#define IRPIN 21
#define EEPROM_IDENT 0x55AA

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
RTC_M41T00S rtc;
IRrecv irrecv(IRPIN);
decode_results results;
DateTime nowtime;

volatile bool pressedButton, batterylow, batSmp,tempSmp,nowtimeSmp;
volatile uint8_t key;
bool lightflag, lightstatus, buzzstatus, hoursalarm, buttonstatus, chargerstatus;

char daysOfTheWeek[7][10] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char dateString[35], ui_buffer[UI_BUFFER_SIZE];

const uint8_t Dimmers[16] = { 5, 15, 25, 35, 50, 65, 85, 105, 125, 145, 165, 185, 200, 215, 235, 255};
const uint8_t kButtonPin = 7, kPowerSwitch = 10, kLedPin = 13, kBuzzerPin = 15, kExtPowerPin = 22;
const int kLightPin = 3, kBatteryPin = A0;

uint32_t  sleepmillis;
uint32_t  displaymillis;
uint32_t  alarmtmillis;
uint32_t  buttonmillis;
uint32_t  delaymillis;
uint8_t   goneMinutes;
uint8_t   goneSeconds;

uint8_t  dim_data = 0;
uint8_t irkey = 0;
uint16_t alarmtime1,alarmtime2;
static uint8_t cBatSmp = 0;
static uint8_t nhours = 0;
static int nBatsum = 0;
static int batVal = 0;
static float tempVal = 0;

void WakeUp() {
  digitalWrite(kPowerSwitch, LOW); 
  detachInterrupt(digitalPinToInterrupt(kButtonPin));
  pressedButton = true;
  while (!digitalRead(kButtonPin)) {};
  sleepmillis =  millis();
}

void setup() {
  ioInit();
  analogReference(INTERNAL2V56);
  Serial.begin(38400);
  rtc.begin();
  rtc.setCalibration(0xA6);
  u8g2.begin();
  u8g2.enableUTF8Print(); 
  tmp112_Init();
//  attachInterrupt(EXTERNAL_INT_7, WakeUp, LOW);
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  adjTimeAlarm();
  getEEPROM();
  key = 0;
  batSmp = true; tempSmp = false; nowtimeSmp = false;
  buttonstatus = true; lightstatus = false; lightflag = true;
    
  rtc.printAllBits();
  i2cdetects(0x03, 0x70);
  irrecv.enableIRIn();
  sleepmillis = millis(); 
}

void loop() {  
  unsigned int uctmp = 0;
  if (pressedButton) 
  {
     batSmp = 1;
     delay(10);
     key = 0;   
    pressedButton = false;
    buttonstatus  = true;
  }
//   Serial.printf("nowtimeSmp  = %02u \n", nowtimeSmp);
   
  if(batSmp)        //battery voltage sensor read 
    {
      uctmp = analogRead(kBatteryPin); 
      delay(10);
        nBatsum += uctmp;
        cBatSmp++;
      if (cBatSmp >= 4)
          {
            batVal = nBatsum >> 2;
            if (batVal < 450) batterylow = true;
            else if (batVal > 460) batterylow = false;
            batVal = map(batVal, 430, 565, 0, 100);
            nBatsum = 0;
            cBatSmp = 0;
          }
      batSmp = 0;
      nowtimeSmp = 0; 
      tempSmp = 1;     
    }
    
  else if(tempSmp)
     {
       tempVal = gettemp();
       tempSmp = 0;
       batSmp = 0;
       nowtimeSmp = 1;
     }
  else if (nowtimeSmp)
      {
        nowtime = rtc.now();
        if(digitalRead(kExtPowerPin)) chargerstatus = true;
        else chargerstatus = false;
        nowtimeSmp = 0;
        tempSmp = 0;
        batSmp = 1;
      }  

   if ((millis() - displaymillis) >= 150) {
            if (key == 0) lcdDisplayAll(); 
            else if (key == 1) lcdDisplayA();
            else if (key == 2) lcdDisplayTemp();
            else if (key == 3) {
                if(lightflag) {
                  lightOn();
                  lightflag = false;
                }
                else {
                  lightOff();
                  lightflag = true;
                }
                key = 0;
            }
            else if (key == 9) {
              updateEEPROM();
              delay(20);
              enterPowerDown();
            }
            else key = 0;
           // Serial.printf("key 5 = %02u \n", key);
            buttonstatus = true;
            displaymillis = millis();
          }
      hoursbuzz();      
      checkButton(); 
      detectIR(); 
      lightAdjust();
   // DateTime future (now + TimeSpan(7,12,30,6));    
}

// void T2_init(){
//  cli();
//  OCR2 = 0x00;
//  TCNT2 =0x00;
//  TCCR2 = 0x03;     //0x03 clkI/O/64 per 2ms! 0x04 clkI/O/256  per 8ms!
//  TIMSK |= (1 << TOIE2);
//  sei(); 
//}

//ISR(TIMER2_OVF_vect)
//{
//      nSysT++;
//      if(nSysT >= SYSTMMAX)  // after 1min reset 
//    {
//      nSysT = 0;
//    }
//    if(!(nSysT % 500)) // after 1sec st + 1 
//    {
//      st++;
//      if (st >= SYSTMMAX) st = 0;
//    }
//}
