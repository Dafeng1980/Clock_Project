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
#define SYSTMMAX 30000

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
RTC_M41T00S rtc;
IRrecv irrecv(IRPIN);
decode_results results;
DateTime nowtime;

volatile bool pressedButton,n,batterylow, batSmp,tempSmp,nowtimeSmp;
static bool buzzstatus = true;
static bool lightstatus = false;
static bool chargerstatus = false;
static bool buttonstatus = true; 
static bool hoursalarm = false;
static bool beepEnable = true;
volatile uint8_t key;
char daysOfTheWeek[7][10] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
char dateString[35];
char ui_buffer[UI_BUFFER_SIZE];
uint16_t alarmtime1,alarmtime2;
const uint8_t kBuzzerPin = 15;
const uint8_t kPowerSwitch = 10;
const int kLightPin = 3;
const uint8_t kButtonPin = 7;
const uint8_t kExtPowerPin = 22;
const uint8_t kLedPin = 13;
const int kBatteryPin = A0;
volatile uint16_t nSysT = 0;
uint16_t nProtT = 0;
uint16_t nShutT = 0;
uint16_t nShowT = 0;
uint16_t st = 0;

static uint8_t cBatSmp = 0;
static uint8_t nhours = 0;
static int nBatsum = 0;
static int batVal = 0;
static float tempVal = 0;
//static uint16_t  nTimeTmp = 0;


void WakeUp() {
  pressedButton = true;
  digitalWrite(kPowerSwitch, LOW);
  while (!digitalRead(kButtonPin)) {};  // wait for button unpress
  detachInterrupt(digitalPinToInterrupt(kButtonPin));
}

void setup() {
  ioInit();
  analogReference(INTERNAL2V56);
  Serial.begin(38400);
  rtc.begin();
  rtc.setCalibration(0xA6);
  u8g2.begin();
  u8g2.enableUTF8Print(); 
  //u8g2.setContrast(0);
  T2_init();
  Tmp112_init();
//  attachInterrupt(EXTERNAL_INT_7, WakeUp, LOW);
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  adjTimeAlarm();
  EEPROM.get(0x00, alarmtime1);
  key = 0;
//  ledstatus = true;
  batSmp = true;
  tempSmp = false;
  nowtimeSmp = false;
  batterylow = false;
  irrecv.enableIRIn();
  rtc.printAllBits();
  setBrightness(); 
  delay(50); 
 // analogWrite(kLightPin, 35);
}

void loop() {  
  unsigned int uctmp = 0;
  uint16_t  nTime = 0;
 // nowtime = rtc.now();  
  if (pressedButton) 
  {
   // while (!digitalRead(kButtonPin)) {};
   // detachInterrupt(digitalPinToInterrupt(kButtonPin));
   // digitalWrite(kPowerSwitch, LOW);
    pressedButton = false;
    delay(10);
    key = 0;
  }
   
  if(batSmp)        //battery voltage sensor read 
    {
//      uctmp = analogRead(kBatteryPin); 
//      delay(20);
      ADCSRA |= bit (ADEN) | bit (ADIF);
      ADMUX = bit (REFS0) | bit (REFS1);
      delay(1);
      set_sleep_mode (SLEEP_MODE_ADC);
      sleep_mode();
      while (bitRead(ADCSRA, ADSC));                         
      //nBatsum += uctmp;
      nBatsum += ADC;
      bitClear (ADCSRA, ADEN);
      cBatSmp++;
      if (cBatSmp >= 4)
          {
            batVal = nBatsum >> 2;
            if (batVal < 435) batterylow = true;
            else if (batVal > 450) batterylow = false;
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
         
    cli();
    nTime = SYSTMMAX + nSysT - nShowT;
    sei();
    if(nTime >= SYSTMMAX)
       {
         nTime = nTime - SYSTMMAX;
       }
          if (nTime >= 100){     
            if (key == 0) lcdDisplayAll();
            else if (key == 1) lcdDisplayA();
            else if (key == 9) enterPowerDown();
            else key = 0;

            cli();
            nShowT = nSysT;
            sei();
            buttonstatus = true;
          }
//   switch (key){
//    case 0:
//      lcdDisplayAll();
//      buttonstatus = true;
//    break;
//    case 1:
//      lcdDisplayA();
//      buttonstatus = true;
//    break;
//    case 2:
//      key = 0;
//      buttonstatus = true;
//    break;
//    case 9:
//      enterPowerDown();
//      buttonstatus = true;
//    break;    
//    default:
//      key = 0;
//      buttonstatus = true;
//   }
      hoursbuzz();      
      checkButton();
      detectIR(); 
   // DateTime future (now + TimeSpan(7,12,30,6));     
}

 void T2_init(){
  cli();
  OCR2 = 0x00;
  TCNT2 =0x00;
  TCCR2 = 0x03;     //0x03 clkI/O/64 per 2ms! 0x04 clkI/O/256  per 8ms!
  TIMSK |= (1 << TOIE2);
  sei(); 
}

ISR(TIMER2_OVF_vect)
{
      nSysT++;
      if(nSysT >= SYSTMMAX)  // after 1min reset 
    {
      nSysT = 0;
    }
    if(!(nSysT % 500)) // after 1sec st + 1 
    {
      st++;
      if (st >= SYSTMMAX) st = 0;
    }
}

//uint16_t getVCC() {
//  uint16_t result = 0;
//  ADCSRA |= bit (ADEN) | bit (ADIF);    // enable ADC, turn off any pending interrupt
//  // set Vcc measurement against 2.56V reference, at A0 ADC0_pin
//  ADMUX = bit (REFS0) | bit (REFS1);
//  delay(1);                             // wait for voltages to settle
//  set_sleep_mode (SLEEP_MODE_ADC);      // sleep during sample for noise reduction
//  for (uint8_t i=0; i<16; i++) {        // get 16 readings
//    sleep_mode();                       // go to sleep while taking ADC sample
//    while (bitRead(ADCSRA, ADSC));      // make sure sampling is completed
//    result += ADC;                      // add them up
//  }
//  bitClear (ADCSRA, ADEN);              // disable ADC  
//  result >>= 4;                         // devide by 16
//  return (1125300L / result);           // 1125300 = 1.1 * 1023 * 1000 
//}
