#define USE_BUILDIN_LED
//#define USE_SERIAL_DEBUG
//#define USE_DISPLAY_DEBUG
#define USE_SAVEPOWER

#include "costumSensor.h" // CapacitiveSensor library copyed in header for modifications //#include <CapacitiveSensor.h>

#include <Narcoleptic.h>
#include "millisTimer.h" // my lib as header for narcoleptic support //#include <MillisTimer.h>


const byte pinLed = 13;

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

// warning, after a long long time there will be an overflow
MillisTimer ignoreSensorCnt;
MillisTimer isButtonPressedCnt;
MillisTimer schedulerCnt;

const int ignoreSensorTime = 950;
const int igoreAfterKl15Off = 550;
const int pressButtonTime = 250;
const float clamp15Treshold = 9.5;

boolean activeKillSwitch = false;
boolean isHandleFloating = false;
boolean isSlzPressSimActive = false;

#include "measureSensor.h"
#include "measureInputs.h"
#include "savePower.h"

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
  Serial.begin(115200);
#endif

#ifdef USE_SAVEPOWER
  // which parts of the uc are not needed?
  //initSavePower();
#endif

}

void camp15Off() {
  if (ignoreSensorCnt.getTime() != 0)
    return;

  // check if hand-on is detected
  if (valueDiff < sensorDifferenz)
    return;

  // simulate pressing for switching On
  digitalWrite(pinOutSlzPulldown, HIGH);

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
  // do nothing if
  if (isButtonPressedCnt.getTime() != 0)
    return;

  if (isSlzPressSimActive == true) {

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
  //writeToSerial();
#endif

#ifdef USE_BUILDIN_LED
  digitalWrite(pinLed, isSlzPressSimActive);
#endif

}

void loop()
{
  if (schedulerCnt.getTime() == 0) {
    // reset Timer, with the next timeout
    schedulerCnt.resetTo(100);

    // do mainThread
    mainThread();
  } else {
    waitTread();
  }
  
}
