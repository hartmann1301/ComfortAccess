#include <MillisCounter.h>
#include <CapacitiveSensor.h>

// input and output pins
const byte pinLed = 13;
const byte pinSensorSend = 2; // pinSensorSend = 2
const byte pinSensorReceive = 4; // pinSensorReceive = 4
const byte pinInKl15 = 9; //pinInKl15 = 10
const byte pinInKillSwitch = 6; //pinInKillSwitch = 11
const byte pinOutSlzRelay = 10; //pinOutSlzRelay = 12
const byte pinOutHeaterRelay = 5; //pinOutHeaterRelay = 9

// init main Timer
//Timer timer;

// warning, after a long long time there will be an overflow
MillisCounter ignoreSensorCnt;
const int ignoreSensorTime = 500;

boolean activeKillSwitch;
boolean activeClamp15;

#include "measureSensor.h"
#include "debugSerial.h"
#include "debugDisplay.h"

void setup()
{
  pinMode(pinLed, OUTPUT);
  pinMode(pinInKl15, INPUT);
  pinMode(pinInKillSwitch, INPUT_PULLUP);
  pinMode(pinOutSlzRelay, OUTPUT);
  pinMode(pinOutHeaterRelay, OUTPUT);

  digitalWrite(pinOutSlzRelay, LOW);
  digitalWrite(pinOutHeaterRelay, LOW);

  // init the OLED debug display
  initDisplay();

  // disable auto calibration
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  
  Serial.begin(9600);
}

void camp15Off() {
  // count
  ignoreSensorCnt.countDown();

  if (ignoreSensorCnt.getValue() != 0)
    return;

  // check if kl15 switch on is detected
  if (valueDiff < sensorDifferenz)
    return;

  // simulate short pressing for switching On
  digitalWrite(pinOutSlzRelay, HIGH);
  Serial.println("press");

  // set onBoard LED
  digitalWrite(pinLed, true);
  delay(200);
  digitalWrite(pinLed, false);

  digitalWrite(pinOutSlzRelay, LOW);
  Serial.println("release");

  // reset to ingore the sensor for a bit
  ignoreSensorCnt.resetTo(ignoreSensorTime);
}

void camp15On() {
  // reset to ingore the sensor
  ignoreSensorCnt.resetTo(ignoreSensorTime);
}

void loop()
{
  doMeasurement();

  // route killSwitch button direkt to Relay
  activeKillSwitch = digitalRead(pinInKillSwitch);
  digitalWrite(pinOutSlzRelay, !activeKillSwitch);

  // if kl15 on, enable heater relay
  activeClamp15 = digitalRead(pinInKl15);
  digitalWrite(pinOutHeaterRelay, activeClamp15);

  // read real klemme 15, main logic
  if (activeClamp15 == false) {
    camp15Off();
  } else {
    camp15On();
  }
  
  printSerial();
  updateDisplay();
}
