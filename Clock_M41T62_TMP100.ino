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

#define speakerPin 12
#define TMP100_ADDR  0x4A
#define M24LC128_ADDR 0x50
#define BATTERY_PIN   A0 
RTC_M41T62 rtc;
ShiftDisplay display(COMMON_ANODE, 8);

int tempo = 120;

volatile bool buttonPressed;
volatile int key;

const int BUTTON_PIN = 2;
const int EXTPOWER_PIN = 4;
char dateString[31];
char tempString[5];
int st, n;
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
	pinMode(BUTTON_PIN, INPUT);
	pinMode(EXTPOWER_PIN, INPUT);
	pinMode(speakerPin, OUTPUT);
	pinMode(0, OUTPUT);
	// attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressInterrupt, FALLING);
	analogReference(INTERNAL1V1);
	buttonPressed = false;

	Serial.begin(38400);
	rtc.begin();
	rtc.checkFlags();
	// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	initTmp100();
	// delay(50);
	sleepMode(SLEEP_POWER_DOWN);
	

	printTime();
	Serial.println("");
	// Serial.println(F("S [S]tart"));
	// Serial.println(F("P sto[P]"));
	Serial.println(F("T set [T]ime"));
	Serial.println(F("A set [A]larm time"));
	Serial.println("");
	Serial.println(F("Choose a menu item:"));
	Serial.println(F("-------------------"));
	batteryval = getBatteryVal();
	Serial.print("BATTER_VAL:=");
	Serial.println(batteryval);
	batteryval = map(batteryval,756, 978, 2, 100);
	Serial.print("BATTER_VAL(%):=");
	Serial.println(batteryval);
	ledlight();

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
			setTime();
			break;
		case 'A':
		case 'a':
			setAlarmTime();
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
	displayAll();
	key = 0;
	n = 0;

	int extp = digitalRead(EXTPOWER_PIN);
	Serial.print(F("EXTPOWER_PIN:="));
	Serial.println(extp);
}


void loop()
{
	if (buttonPressed) {
		detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
		displayAll();
		key = 0;
		n = 0;
		buttonPressed = false;
	}
	switch (key)
	{
	case 0 :
		displayTime();
			break;
	case 1 :
		displayTimeA();
		break;
	case 2 :
		displayDateA();
		break;
	case 3 :
		displayTemp();
		break;
	case 4 :
		displayAll();
	default:
		key = 0;
		break;
	}

	if (n == 900)
	{
		displayAll();
		n = 0;
		attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPressInterrupt, FALLING);
		delay(10);
		sleep();
	}
	n++;
	checkButton();
	Serial.print("n:=");
	Serial.println(n);
}

void initTmp100() {
	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0x01);
	Wire.write(0x31);  // 12 bits Resolution(320ms); in Shutdown Mode
	Wire.endTransmission();
}

void displayTime() {
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

		displayTemp();
		if (digitalRead(EXTPOWER_PIN)) {
			n = 0;
		}
	}
}
void displayTimeA() {
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
	if (digitalRead(EXTPOWER_PIN)) {
		n = 0;
	}
}
void displayDateA() {
	DateTime now = rtc.now();
	sprintf(dateString, "%02u-%02u d%01u", now.month(), now.day(),now.dayOfWeek());
	display.set(dateString);
	display.show(2000);
	if (digitalRead(EXTPOWER_PIN)) {
		n = 0;
	}
}
void displayTemp() {
	uint8_t data[2];
	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0x01);
	Wire.endTransmission();
	Wire.requestFrom(TMP100_ADDR, 1);
	uint8_t cros = Wire.read();
	bitWrite(cros, 6, 1);               //One-shot Temperature

	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0x01);
	Wire.write(cros);
	Wire.endTransmission();
	delay(320);

	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0X00);
	Wire.endTransmission();
	Wire.requestFrom(TMP100_ADDR, 2);

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
	if (digitalRead(EXTPOWER_PIN)) {
		n = 0;
	}
}
void displayAll() {
	uint8_t data[2];
	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0x01);
	Wire.endTransmission();
	Wire.requestFrom(TMP100_ADDR, 1);
	uint8_t cros = Wire.read();
	bitWrite(cros, 6, 1);               //One-shot Temperature

	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0x01);
	Wire.write(cros);
	Wire.endTransmission();
	delay(320);

	Wire.beginTransmission(TMP100_ADDR);
	Wire.write(0X00);
	Wire.endTransmission();
	Wire.requestFrom(TMP100_ADDR, 2);

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

	batteryval = getBatteryVal();
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

void printTime()
{
	DateTime time = rtc.now();

	sprintf(dateString, "%4u-%02u-%02u %02u:%02u:%02u .",
		time.year(), time.month(), time.day(), time.hour(),
		time.minute(), time.second());
	Serial.println(dateString);
}

void setTime()
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
void setAlarmTime() {
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
	y = Serial.read(); // hour: ones digit
	hour = 10 * (x - '0') + (y - '0');

	x = Serial.read(); // discard spacer
	x = Serial.read(); // min: tens digit
	y = Serial.read(); // min: ones digit
	min = 10 * (x - '0') + (y - '0');
	x = Serial.read(); //  discard spacer
	y = Serial.read(); // alarmRepeat mode
	mode = y - '0';
	delay(10);
	Serial.flush();
	DateTime alarmTime(2019, month, day, hour, min, sec);
	rtc.alarmSet(alarmTime);
	rtc.alarmRepeat(mode);

	Serial.println(F("Set Successful"));
	//rtc.alarmRepeat(4);// set alarm repeat mode (once per day)
	//DateTime alarmTime(2019, month, day, hour, min, sec);
	// printTimeo(alarmTime);

	// rtc.alarmEnable(1);
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


void checkButton() {
	
	if (digitalRead(BUTTON_PIN) == 0) {
		delay(10);
		if (digitalRead(BUTTON_PIN) == 0);
		{
			if (rtc.checkFlags()) {

				//rtc.printAllBits();
				// delay(50);
				playMusic();
				key = 0;
			}
			else
			key++;
			n = 0;
		}
	}
	
}
void ledlight() {
	for (int i = 1; i < 10; i++) {
		digitalWrite(0, HIGH);
		delay(300);
		digitalWrite(0, LOW);
		delay(300);

	}
 }

int getBatteryVal() {
	 int sum=0;
	for (int i = 0; i < 5; i++) {
		sum+= analogRead(BATTERY_PIN);
		//Serial.print("Sum:=");
		//Serial.println(sum);
	}
	return sum = sum / 5;
}

void playMusic(){
	for (int i = 0; i < 72; i++) {
		note(MusicReadEeprom(2 * i), EIGHTHNOTE);
	}
	rest(EIGHTHNOTE);
	/////// KEEP ALL CODE BELOW UNCHANGED, CHANGE VARS ABOVE ////////
	noTone(speakerPin);
}
void spacedNote(int frequencyInHertz, int noteLength)
{
	tone(speakerPin, frequencyInHertz);
	float delayTime = (1000 / tempo) * (60 / 4) * noteLength;
	delay(delayTime - 50);
	noTone(speakerPin);
	delay(50);
}
void note(int frequencyInHertz, int noteLength)  //Code to take care of the note
{
	tone(speakerPin, frequencyInHertz);
	float delayTime = (1000 / tempo) * (60 / 4) * noteLength;
	delay(delayTime);
}
void rest(int restLength)
{
	noTone(speakerPin);
	float delayTime = (1000 / tempo) * (60 / 4) * restLength;
	delay(delayTime);
}

int MusicReadEeprom(uint16_t address) {
	int data;
	Wire.beginTransmission(M24LC128_ADDR);      
	Wire.write((int)(address >> 8));        
	Wire.write((int)(address & 0xFF));              
	Wire.endTransmission(I2C_NOSTOP);       
	Wire.requestFrom(M24LC128_ADDR, 2);   
	uint8_t dataL = Wire.read();
	uint8_t dataH = Wire.read(); 
	data = dataH * 256 + dataL;
	return data;                            
}
void M24LC128writeByte(uint16_t address, uint8_t  data)
{
	Wire.beginTransmission(M24LC128_ADDR); 
	Wire.write((int)(address >> 8));             
	Wire.write((int)(address & 0xFF));                
	Wire.write(data);                        
	Wire.endTransmission();                   
	delay(6);
}
void M24LC128writeBytes(uint16_t address, uint8_t count, uint8_t * dest)
{
	if (count > 64) {
		count = 64;
	//	Serial.print("Page count cannot be more than 64 bytes!");
	}

	Wire.beginTransmission(M24LC128_ADDR);   
	Wire.write((int)(address >> 8));
	Wire.write((int)(address & 0xFF));
	for (uint8_t i = 0; i < count; i++) {
		Wire.write(dest[i]);                     
	}
	Wire.endTransmission();                   
}
uint8_t M24LC128readByte(uint16_t address)
{
	uint8_t data;
	Wire.beginTransmission(M24LC128_ADDR);       
	Wire.write((int)(address >> 8));               
	Wire.write((int)(address & 0xFF));                
	Wire.endTransmission(I2C_NOSTOP);        
	Wire.requestFrom(M24LC128_ADDR, 1);   
	data = Wire.read();                      
	return data;                             
}
void M24LC128readBytes(uint16_t address, int count, uint8_t * dest)
{
	Wire.beginTransmission(M24LC128_ADDR);      
	Wire.write((int)(address >> 8));
	Wire.write((int)(address & 0xFF));
	Wire.endTransmission(I2C_NOSTOP);         
	uint8_t i = 0;
	Wire.requestFrom(M24LC128_ADDR, count);
	while (Wire.available()) {
		dest[i++] = Wire.read();
	}              
}
