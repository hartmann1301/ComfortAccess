#define USE_BUILDIN_LED
#define USE_PIEZO_SPEAKER

#define USE_DISPLAY_DEBUG
//#define USE_SERIAL_DEBUG

//#define USE_SAVEPOWER //problem with save power?????

#define K21
//#define K48

#include <MillisTimer.h>
#include <CapacitiveSensor.h>

#include "pinsParameter.h"
#include "savePower.h"
#include "measureSensor.h"
#include "measureInputs.h"
#include "toneHelper.h"

#ifdef USE_DISPLAY_DEBUG
#include "startScreens.h"
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
  initMeasureSensor();

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

  setModeAwake();

  // simulate pressing for switching On
  digitalWrite(pinOutSlzPulldown, HIGH);

  // press the button, this trigger the reset function as well
  isSlzPressSimActive = true;
  pressSlzCounter++;

  pressButtonCnt.resetTo(pressButtonTime);

  // reset to ingore the sensor for a bit
  ignoreSensorCnt.raiseTo(ignoreAfterPushSim);
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
    
    // first time afte clamp switch on
    if (isClamp15Off != wasClamp15Off) {
      kl15OffOffset = Narcoleptic.millis() + millis();

      // measure the temp once at clamp change
      //measureHandleTemp();
      
      // we woke up by someone else, other ecus are awake
      setModeAwake();

      // this sets the heater free, mosfet lock
      digitalWrite(pinOutSetHeaterFree, false);

      /* wait for the first time, because V-BCx could need time to discarge
      while (voltageBCx > 4.0) {
        measureBCx();
      }
      */

      //delay(1);
    }

    // this should not be necassary, just to be 100% sure the mosfets lock while measuring
    digitalWrite(pinOutSetHeaterFree, false);

    measureSensor();

    // this can change the interval time
    checkBCxSwitchOff();

    camp15Off();

    kl15OffCounter = (Narcoleptic.millis() + millis()) - kl15OffOffset ;

  } else {
    // this enables the heaters to heat, mosfets have low Ohms
    //digitalWrite(pinOutSetHeaterFree, true); !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! no heating

    // no measuring, so set duration to 0, this is just for the oled display
    measureDuration = 0;
    kl15OffCounter = 0;

    // reset timer to ingore the sensor and disalbe the filter
    ignoreSensorCnt.raiseTo(igoreAfterKl15Off);

  }
  
  // this boolean is used to find out changes at clamp15
  wasClamp15Off = isClamp15Off;

  // reset the slz Button if necassary
  checkSlzButton();

#ifdef USE_BUILDIN_LED
  digitalWrite(pinLed, isSlzPressSimActive);
#endif

#ifdef USE_PIEZO_SPEAKER
  checkTones();
#endif

#ifdef USE_DETECTHAND
  checkHand();
#endif

#ifdef USE_SERIAL_DEBUG
  writeToSerial();
#endif

#ifdef USE_DISPLAY_DEBUG
  writeToDisplay();
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
