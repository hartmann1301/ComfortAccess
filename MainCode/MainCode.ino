#define USE_BUILDIN_LED
#define USE_DISPLAY_DEBUG

// its savepower or serial debugging
#define USE_SERIAL_DEBUG
//#define USE_SAVEPOWER

//#define USE_DETECTHAND
#define USE_HANDLETEMP
//#define USE_BREAKOUT_NANO // old seltmade platine with nano breakout board

#include "millisTimer.h" // my timer-lib included as header because of narcoleptic support //#include <MillisTimer.h>
#include "costumSensor.h" // CapacitiveSensor library copyed in header for modifications //#include <CapacitiveSensor.h>
#include "pinsParameters.h"
#include "savePower.h"
#include "measureSensor.h"
#include "measureInputs.h"
#include "detectHand.h"

#ifdef USE_DISPLAY_DEBUG
#include "debugDisplay.h"
#endif

#ifdef USE_SERIAL_DEBUG
#include "debugSerial.h"
#endif

#ifdef USE_SAVEPOWER
#ifdef USE_SERIAL_DEBUG
#error F("Error: savepower and serialDebug confict, because uc start to sleep before it finished the serial print")
#endif
#endif

void setup()
{
  initPinsAndClassses();

#ifdef USE_DISPLAY_DEBUG
  // init the OLED debug display
  initDisplay();
#endif

#ifdef USE_SERIAL_DEBUG
  initSerial();
#endif

#ifdef USE_SAVEPOWER
  // which parts of the uc are not needed?
  initSavePower();
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

  pressButtonCnt.resetTo(pressButtonTime);

  // reset to ingore the sensor for a bit
  ignoreSensorCnt.resetTo(ignoreAfterPushSim);
}

void checkSlzButton() {
  // do nothing if it is not long enough pressed
  if (pressButtonCnt.getTime() != 0)
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
  boolean activeKillSwitch = digitalRead(pinInKillSwitch);
  digitalWrite(pinOutSlzPulldown, !activeKillSwitch);

  // read real klemme 15, main logic
  isClamp15Off = (voltageClamp15 < clamp15Treshold);
  if (isClamp15Off) {

#ifdef USE_HANDLETEMP
    // measure the temp once at clamp change
    if (isClamp15Off != wasClamp15Off)
      measureHandleTemp();
#endif

    // this sets the heater free, mosfet lock
    digitalWrite(pinOutSetHeaterFree, false);

    measureSensor();

    // this is for BCx switchOff detection, ignore sensor this timeslot
    if (voltageBCx > 3.0) {
      ignoreSensorCnt.resetTo(3050); // mostly for the oled screen
      disableSensorCnt.resetTo(2050);
    }

    camp15Off();

  } else {
    // this enables the heaters to heat, mosfets have low Ohms
    digitalWrite(pinOutSetHeaterFree, true);

    // no measuring, so set duration to 0, this is just for the oled display
    measureDuration = 0;

    // reset timer to ingore the sensor and disalbe the filter
    ignoreSensorCnt.resetTo(igoreAfterKl15Off);
    disableSensorCnt.resetTo(igoreAfterKl15Off);
  }
  // this boolean is used to find out changes at clamp15
  wasClamp15Off = isClamp15Off;

  // reset the slz Button if necassary
  checkSlzButton();

#ifdef USE_DISPLAY_DEBUG
  writeToDisplay();
#endif

#ifndef USE_SAVEPOWER
  writeToSerial();
#endif

#ifdef USE_BUILDIN_LED
  digitalWrite(pinLed, isSlzPressSimActive);
#endif

#ifdef USE_DETECTHAND
  checkHand();
#endif
}

void loop()
{
  if (schedulerCnt.getTime() == 0) {
    // reset Timer, with the next timeout
    schedulerCnt.resetTo(mainIntervalTime);

    // do mainThread
    mainThread();
  } else {
    waitTread();
  }
}
