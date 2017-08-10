#include <Counter.h>
#include <CapacitiveSensor.h>

const byte pinLed = 13;
const byte pinSensorSend = 2;
const byte pinSensorReceive = 4;
const byte pinKillSwitchIn = 14;  // 14 <=> A0

CapacitiveSensor capSensor = CapacitiveSensor(pinSensorReceive, pinSensorSend);

// filter variables and function
void calcFilter(float &value, int newValue, const int filterFaktor) {
  value = ((value * filterFaktor) + newValue) / (filterFaktor + 1.0);
}

// parameter
const int sensorThreshold = 15;
const int sensorDifferenz = 20;

const int timeoutKl15Off = 1000;
const int timeoutHandlebarLock = 2000;

// globals
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;

Counter killSwitchCnt;

long measureStart = 0;
long measureDuration = 0;

enum { KL15_OFF, KL15_ON};
enum { HANDLEBAR_UNLOCKED, HANDLEBAR_LOCKED};
byte statusKl15 = KL15_OFF;
byte statusLock = HANDLEBAR_UNLOCKED;

void setup()
{
  Serial.begin(9600);

  pinMode(pinLed, OUTPUT);  
  pinMode(pinKillSwitchIn, INPUT_PULLUP);
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

    if (!digitalRead(pinKillSwitchIn)) {
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
