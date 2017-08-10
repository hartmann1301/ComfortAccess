#include <Counter.h>
#include <CapacitiveSensor.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

const byte pinLed = 13;

const byte pinSensorSend = 2;
const byte pinSensorReceive = 3;

const byte pinInKillSwitch = 11;
const byte pinOutSLZpullDown = 10;

const byte pinOutDisplayGnd = A2
const byte pinOutDisplayVcc = A3
//const byte pinOutDisplaySda = A4
//const byte pinOutDisplayScl = A5


CapacitiveSensor capSensor = CapacitiveSensor(pinSensorReceive, pinSensorSend);

// filter variables and function
void calcFilter(float &value, int newValue, const int filterFaktor) {
  value = ((value * filterFaktor) + newValue) / (filterFaktor + 1.0);
}

// parameter
const int sensorThreshold = 15;
const int sensorDifferenz = 20;

const int timeoutKl15Off = 100;
const int timeoutHandlebarLock = 1500;

// globals
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;

Counter killSwitchCnt;
Counter ignoreSensorCnt;

long measureStart = 0;
long measureDuration = 0;

enum { KL15_OFF, KL15_ON};
enum { HANDLEBAR_UNLOCKED, HANDLEBAR_LOCKED};
byte statusKl15 = KL15_OFF;
byte statusLock = HANDLEBAR_UNLOCKED;

void setup()
{


  pinMode(pinLed, OUTPUT);  

  pinMode(pinInKillSwitch, INPUT_PULLUP);

  // set display pins
  pinMode(pinOutDisplayGnd, OUTPUT);
  pinMode(pinOutDisplayVcc, OUTPUT);
  digitalWrite(pinOutDisplayGnd, LOW);
  digitalWrite(pinOutDisplayVcc, HIGH);

  pinMode(pinOutSLZpullDown, OUTPUT);  
  digitalWrite(pinOutSLZpullDown, HIGH);  

  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  // horizontal line under textblock
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Cap. Sensor Test");
  display.drawLine(79, 35, 127, 35, WHITE);
  display.drawLine(1, 35, 48, 35, WHITE);
  display.display();
}


void loop()
{
  // do capa measurement
  measureStart = millis();
  valueCurrent =  (float) capSensor.capacitiveSensor(50);
  measureDuration = millis() - measureStart;

  // store average value in first parameter
  calcFilter(valueAverage, valueCurrent, 10);
  calcFilter(valueSlow, valueCurrent, 50);

  // count time killSwitch is pressed
  if (statusLock == HANDLEBAR_LOCKED) {
    killSwitchCnt.reset();
    
  } else {

    if (!digitalRead(pinInKillSwitch)) {
      killSwitchCnt.count();

    } else {
      killSwitchCnt.reset();
          
    }
  }

  // main logic
  if (statusKl15 == KL15_OFF) {
    // kl15 is Off

    // check if kl15 switch on is detected
    if (valueAverage > valueSlow + sensorDifferenz) {   // diff of average and slow value
      //if (valueAverage > sensorThreshold) {   // fixed threshold value

      digitalWrite(pinOutSLZpullDown, LOW);
      delay(100);
      digitalWrite(pinOutSLZpullDown, HIGH);
      
      statusKl15 = KL15_ON;
    }

    // check if handlebar needs to be locked
    if (killSwitchCnt.getValue() > timeoutHandlebarLock) {
      statusLock = HANDLEBAR_LOCKED;
    }

  } else {
    // kl15 is On

    // when kl15 is on handlebar is unlocked
    statusLock = HANDLEBAR_UNLOCKED;

    // check if kl15 switch off is detected
    if (killSwitchCnt.getValue() > timeoutKl15Off) {
      statusKl15 = KL15_OFF;
    }

  }
  //if (statusKl15 == KL15_OFF) {
  digitalWrite(pinLed, statusKl15);

  // print to Serial
  Serial.print((int) valueAverage);
  Serial.print("\t");

  Serial.print((int) valueSlow);
  Serial.print("\t");

  if (statusKl15 == KL15_OFF) {
    Serial.print("Kl15_off");
  } else {
    Serial.print("Kl15_on ");
  }
  Serial.print("\t");


  if (statusLock == HANDLEBAR_UNLOCKED) {
    Serial.print("UNLOCKED");
  } else {
    Serial.print("LOCKED  ");
  }
  Serial.print("\t");

  Serial.print(killSwitchCnt.getValue());
  Serial.print("\n");

  delay(10);
}
