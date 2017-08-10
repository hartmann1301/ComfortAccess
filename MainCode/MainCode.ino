#include <MillisCounter.h>
#include <CapacitiveSensor.h>
#include <Timer.h>

#define USE_BUILDIN_LED
#define USE_SERIAL_DEBUG
#define USE_DISPLAY_DEBUG

#ifdef USE_BUILDIN_LED
const byte pinLed = 13;
#endif

// measurement sensor pins
const byte pinSensorSend = 4;
const byte pinSensorReceive = 2;

// measurement inputs pins
const byte pinInKl15 = A0;
const byte pinInBCx = A1;
const byte pinInHandleTemp = A7;
const byte pinInKillSwitch = 6;

// 0 = mosfet sperrt, 1 = mosfet zieht gegen GND und simuliert taster
const byte pinOutSlzPulldown = 10;

// 0 = heizgriff free floating für messung, 1 = heizgriff mit GND und BCx verbunden
const byte pinOutSetHeaterFree = 5;

// init sensor pin
CapacitiveSensor capSensor = CapacitiveSensor(pinSensorSend, pinSensorReceive);

// init main Timer
Timer timer;

// warning, after a long long time there will be an overflow
MillisCounter ignoreSensorCnt;
MillisCounter isButtonPressedCnt;
MillisCounter mainTimer;

const int ignoreSensorTime = 950;
const int igoreAfterKl15Off = 550;
const int pressButtonTime = 250;
const float clamp15Treshold = 9.5;

boolean activeKillSwitch = false;
boolean isHandleFloating = false;
boolean isSlzPressSimActive = false;

#include "measureSensor.h"
#include "measureInputs.h"

#ifdef USE_DISPLAY_DEBUG
#include "debugDisplay.h"
#endif

#ifdef USE_SERIAL_DEBUG
#include "debugSerial.h"
#endif

void setup()
{
  pinMode(pinInKl15, INPUT);

  const byte pinInKl15 = A0;
  const byte pinInBCx = A1;
  const byte pinInHandleTemp = A7;
  const byte pinInKillSwitch = 6;

  pinMode(pinInKillSwitch, INPUT_PULLUP);

  pinMode(pinLed, OUTPUT);
  pinMode(pinOutSlzPulldown, OUTPUT);
  pinMode(pinOutSetHeaterFree, OUTPUT);

  // set outputs
  digitalWrite(pinOutSlzPulldown, LOW);
  digitalWrite(pinOutSetHeaterFree, LOW);

  // init my counters
  ignoreSensorCnt.resetTo(0);
  isButtonPressedCnt.resetTo(0);

  // set Analog Reference Voltage to 1.1 volts
  analogReference(INTERNAL);

  // disable auto calibration
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);

  // init Timer with main Thread
  //timer.every(100, mainThread);

#ifdef USE_DISPLAY_DEBUG
  // init the OLED debug display
  initDisplay();
#endif

#ifdef USE_SERIAL_DEBUG
  // start Serial debug
  Serial.begin(9600);
#endif
}

void camp15Off() {
  // count  down
  ignoreSensorCnt.countDown();

  if (ignoreSensorCnt.getValue() != 0)
    return;

  // check if hand-on is detected
  if (valueDiff < sensorDifferenz)
    return;

  // simulate pressing for switching On
  digitalWrite(pinOutSlzPulldown, HIGH);
  Serial.println("press");

  // press the button, this trigger the reset function as well
  isSlzPressSimActive = true;

  isButtonPressedCnt.resetTo(pressButtonTime);

  // reset to ingore the sensor for a bit
  ignoreSensorCnt.resetTo(ignoreSensorTime);
}

void camp15On() {
  // reset to ingore the sensor
  ignoreSensorCnt.resetTo(ignoreSensorTime);
}

void checkSlzButton() {
  // update internal counter
  isButtonPressedCnt.countDown();

  // do nothing if 
  if (isButtonPressedCnt.getValue() != 0)
    return;

  if (isSlzPressSimActive == true) {
    Serial.println("release Slz Button");
    isSlzPressSimActive = false;

    digitalWrite(pinOutSlzPulldown, LOW);
  }
}

void mainThread() {

  measureClamp15();
  measureBCx();

  // route killSwitch button direkt to Mosfet
  activeKillSwitch = digitalRead(pinInKillSwitch);
  digitalWrite(pinOutSlzPulldown, !activeKillSwitch);

  // read real klemme 15, main logic
  if (voltageClamp15 < clamp15Treshold) {

    if (isHandleFloating == false)
      ignoreSensorCnt.resetTo(igoreAfterKl15Off);

    isHandleFloating = true;

    digitalWrite(pinOutSetHeaterFree, false);

    measureSensor();

    camp15Off();

  } else {
    isHandleFloating = false;

    digitalWrite(pinOutSetHeaterFree, true);

    camp15On();
  }

  // reset the slz Button if necassary
  checkSlzButton();

#ifdef USE_DISPLAY_DEBUG
  writeToDisplay();
#endif

#ifdef USE_SERIAL_DEBUG
  writeToSerial();
#endif

#ifdef USE_BUILDIN_LED
  digitalWrite(pinLed, isSlzPressSimActive);
#endif

}

void loop()
{
  // check the current time und count
  mainTimer.countDown();

  if (mainTimer.getValue() == 0) {
    // reset Timer with the current Timer
    mainTimer.resetTo(100);

    // do mainThread
    mainThread();
  }
}
