// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       newClock_M41T62.ino
    Created:	2019/6/29 9:23:56
    Author:     DAFENG\Dafeng
*/

#include <Wire.h>
#include "M41T62.h"
#include <ShiftDisplay.h>

#define SIXTEENTHNOTE 1
#define EIGHTHNOTE 2
#define DOTTEDEIGHTNOTE 3
#define QUARTERNOTE 4
#define DOTTEDQUARTERNOTE 6
#define HALFNOTE 8
#define DOTTEDHALFNOTE 12
#define WHOLENOTE 16

#define I2C_NOSTOP 0

RTC_M41T62 rtc;
ShiftDisplay display(14, 15, 13, COMMON_ANODE, 8);

volatile bool buttonPressed;
volatile int key;

const int kTempo = 120;
const int kM24lc128Addr = 0x50;
const int kTmp100Addr = 0x4A;
const int kButtonPin = 2;
const int kExtPowerPin = 4;
const int kSpeakerPin = 12;
const int kBatteryPin = A7;


char dateString[31];
char tempString[5];
int n;
int batteryval;


void buttonPressInterrupt() {
	
	buttonPressed = true;
	noSleep();
}

void setup()
{
	uint8_t k;
//#ifdef AVR
//	Wire.begin();
//#else
//	Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
//#endif
	pinMode(kButtonPin, INPUT);
	pinMode(kExtPowerPin, INPUT);
	pinMode(kSpeakerPin, OUTPUT);
	pinMode(0, OUTPUT);
	// attachInterrupt(digitalPinToInterrupt(kButtonPin), buttonPressInterrupt, FALLING);
	analogReference(INTERNAL1V1);
	buttonPressed = false;

	Serial.begin(38400);
	rtc.begin();
	rtc.checkFlags();
	// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	InitTmp100();
	sleepMode(SLEEP_POWER_DOWN);
	

	PrintTime();
	Serial.println("");
	// Serial.println(F("S [S]tart"));
	// Serial.println(F("P sto[P]"));
	Serial.println(F("T set [T]ime"));
	Serial.println(F("A set [A]larm time"));
	Serial.println("");
	Serial.println(F("Choose a menu item:"));
	Serial.println(F("-------------------"));
	batteryval = getbatteryval();
	Serial.print("BATTER_VAL:=");
	Serial.println(batteryval);
	batteryval = map(batteryval,756, 978, 2, 100);
	Serial.print("BATTER_VAL(%):=");
	Serial.println(batteryval);
	LedLight();

	if (Serial.available() > 0)
	{
		k = Serial.read();
		switch (k)
		{
		//case 'S':
		//case 's':
		//	rtc.setStop(1);
		//	break;

		//case 'P':
		//case 'p':
		//	rtc.setStop(0);
		//	break;
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
	key = 0;
	n = 0;

	int extp = digitalRead(kExtPowerPin);
	Serial.print(F("kExtPowerPin:="));
	Serial.println(extp);
}


void loop()
{
	if (buttonPressed) {
		detachInterrupt(digitalPinToInterrupt(kButtonPin));
		DisplayAll();
		key = 0;
		n = 0;
		buttonPressed = false;
	}
	switch (key)
	{
	case 0 :
		DisplayTime();
			break;
	case 1 :
		DisplayTimeA();
		break;
	case 2 :
		DisplayDate();
		break;
	case 3 :
		DisplayTemp();
		break;
	case 4 :
		DisplayAll();
	default:
		key = 0;
		break;
	}

	if (n == 600)
	{
		DisplayAll();
		n = 0;
		attachInterrupt(digitalPinToInterrupt(kButtonPin), buttonPressInterrupt, FALLING);
		delay(10);
		sleep();
	}
	n++;
	CheckButton();
	//Serial.print("n:=");
	//Serial.println(n);
}

void InitTmp100() {
	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0x01);
	Wire.write(0x31);  // 12 bits Resolution(320ms); in Shutdown Mode
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

	if (!((n+1)%36)) {

		DisplayTemp();
		if (digitalRead(kExtPowerPin)) {
			n = 0;
		}
	}
}
void DisplayTimeA() {
	uint8_t dow;
	DateTime now = rtc.now();
	sprintf(dateString, "%02u d%01u %02u", now.hour(), now.dayOfWeek(), now.minute());
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
	if (digitalRead(kExtPowerPin)) {
		n = 0;
	}
}
void DisplayDate() {
	DateTime now = rtc.now();
	sprintf(dateString, "%02u-%02u d%1u", now.month(), now.day(),now.dayOfWeek());
	display.set(dateString);
	display.show(2000);
	if (digitalRead(kExtPowerPin)) {
		n = 0;
	}
}
void DisplayTemp() {
	uint8_t data[2];
	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0x01);
	Wire.endTransmission();
	Wire.requestFrom(kTmp100Addr, 1);
	uint8_t cros = Wire.read();
	bitWrite(cros, 6, 1);               //One-shot Temperature

	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0x01);
	Wire.write(cros);
	Wire.endTransmission();
	delay(320);

	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0X00);
	Wire.endTransmission();
	Wire.requestFrom(kTmp100Addr, 2);

	if (Wire.available() == 2) {
		data[0] = Wire.read();
		data[1] = Wire.read();
	}
	int temp1 = ((data[0] * 256) + data[1]) / 16;      // (msb << 8 |lsb) >> 4;
	if (temp1 >= 2048) {
		temp1 -= 4096;
	}
	int dot = (temp1 & 0x00F)*0.625;
	int fTemp = temp1 * 0.0625;
	sprintf(tempString, "%02u#%01uC", fTemp, dot);
	display.show(tempString, 3000, ALIGN_CENTER);
	String Null = "        ";
	display.set(Null);
	display.show();
	if (digitalRead(kExtPowerPin)) {
		n = 0;
	}
}
void DisplayAll() {
	uint8_t data[2];
	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0x01);
	Wire.endTransmission();
	Wire.requestFrom(kTmp100Addr, 1);
	uint8_t cros = Wire.read();
	bitWrite(cros, 6, 1);               //One-shot Temperature

	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0x01);
	Wire.write(cros);
	Wire.endTransmission();
	delay(320);

	Wire.beginTransmission(kTmp100Addr);
	Wire.write(0X00);
	Wire.endTransmission();
	Wire.requestFrom(kTmp100Addr, 2);

	if (Wire.available() == 2) {
		data[0] = Wire.read();
		data[1] = Wire.read();
	}
	int temp1 = ((data[0] * 256) + data[1]) / 16;      // (msb << 8 |lsb) >> 4;
	if (temp1 >= 2048) {
		temp1 -= 4096;
	}
	int dot = (temp1 & 0x00F)*0.625;
	int fTemp = temp1 * 0.0625;

	batteryval = getbatteryval();
	batteryval = map(batteryval, 756, 978, 2, 100);
	DateTime time = rtc.now();
	sprintf(dateString, " %4u-%02u-%02u d%1u %02u_%02u  %2u#%1uC P%2u ", time.year(), time.month(), time.day(), time.dayOfWeek(), time.hour(), time.minute(),fTemp, dot, batteryval);

	String condition = dateString;
	condition = "        " + condition;

	while (condition.length() > 0) {
		display.show(condition, 650, ALIGN_LEFT);
		condition.remove(0, 1);
	}
}

void PrintTime()
{
	DateTime time = rtc.now();

	sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
		time.year(), time.month(), time.day(), time.hour(),
		time.minute(), time.second());
	Serial.println(dateString);
}

void SetTime()
{
	uint8_t x;
	char date[11], time[8];
	Serial.println(F("Enter date format"));
	Serial.println(F("mmm-dd-yyyy hh-mm-ss sample input: Dec 26 2009 12:34:56"));

	while (Serial.available() < 20)
	{
	}
	for (int i = 0; i < 11; i++) {
		date[i] = Serial.read();
	}
	x = Serial.read();
	for (int i = 0; i < 8; i++) {
		time[i] = Serial.read();
	}

delay(10);
Serial.flush();
rtc.adjust(DateTime(date, time));
delay(10);
Serial.println(date);
DateTime now=rtc.now();
x = now.dayOfWeek();
//Serial.print("X=");
//Serial.println(x);
Wire.beginTransmission(0x68);   //M41T62_ADDRESS
Wire.write(0x04);              // SQW Frequency / Day of Week
Wire.write(x);
Wire.endTransmission();

Serial.println(F("Set Successful"));
}
void SetAlarmTime() {
	uint8_t x, y;
	uint8_t sec, min, hour, day, month, mode;
	DateTime now = rtc.now();
	month = now.month();
	day = now.day();
	sec = 0;
	
	Serial.println(F("Enter alarm time format (mode is 1 to 6, 4 is per day)"));
	Serial.println(F("hh:mm mode"));

	while (Serial.available() < 8)
	{
	}
	x = Serial.read(); // hour: tens digit
	Serial.write(x);
	y = Serial.read(); // hour: ones digit
	Serial.write(y);
	hour = 10 * (x - '0') + (y - '0');

	x = Serial.read(); // discard spacer
	Serial.write(x);
	x = Serial.read(); // min: tens digit
	Serial.write(x);
	y = Serial.read(); // min: ones digit
	Serial.write(y);
	min = 10 * (x - '0') + (y - '0');

	x = Serial.read(); //  discard spacer
	Serial.write(x);
	y = Serial.read(); // alarmRepeat mode
	Serial.write(y);
	Serial.println(" ");
	mode = y - '0';

	delay(10);
	Serial.flush();
	DateTime alarmTime(2019, month, day, hour, min, sec);
	rtc.alarmSet(alarmTime);
	rtc.alarmRepeat(mode);
	Serial.println(F("Set Successful"));
	//rtc.alarmRepeat(4);// set alarm repeat mode (once per day)
}

//void displayDate()
//{
//	DateTime time = rtc.now();
//	display.show(100);
//	buttonPressed = false;
//	sprintf(dateString, " %4u-%02u-%02u ", time.year(), time.month(), time.day());
//	String condition = dateString;
//	condition = "        " + condition;
//	while (condition.length() > 0) {
//		display.show(condition, 650, ALIGN_LEFT);
//		condition.remove(0, 1);
//	}
//}


void CheckButton() {
	
	if (digitalRead(kButtonPin) == 0) {
		delay(5);
		if (digitalRead(kButtonPin) == 0);
		{
			if (rtc.checkFlags()) {

				//rtc.printAllBits();
				// delay(50);
				PlayMusic();
				key = 0;
			}
			else
			key++;
			n = 0;
		}
	}
	
}
void LedLight() {
	for (int i = 1; i < 10; i++) {
		digitalWrite(0, HIGH);
		delay(300);
		digitalWrite(0, LOW);
		delay(300);

	}
 }

int getbatteryval() {
	 int sum=0;
	for (int i = 0; i < 5; i++) {
		sum+= analogRead(kBatteryPin);
		//Serial.print("Sum:=");
		//Serial.println(sum);
	}
	return sum = sum / 5;
}

void PlayMusic(){
	for (int i = 0; i < 72; i++) {
		note(musicreadeeprom(2 * i), EIGHTHNOTE);
	}
	rest(EIGHTHNOTE);
	/////// KEEP ALL CODE BELOW UNCHANGED, CHANGE VARS ABOVE ////////
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
	//	Serial.print("Page count cannot be more than 64 bytes!");
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
