#include <Narcoleptic.h>

void initSavePower() {
  // Do not disable millis - we need it for our delay() bzw. millis() function.
  //Narcoleptic.disableMillis();

  // adc is needed for voltage measuring
  //Narcoleptic.disableADC();

#ifndef USE_DISPLAY_DEBUG
  // wire is only used for I2C communication with the oled display
  Narcoleptic.disableWire();
#endif

#ifndef USE_SERIAL_DEBUG
  // serial is only used for serial communication
  Narcoleptic.disableSerial();
#endif

  // always disalbe the following parts of the uc
  Narcoleptic.disableSPI();

#ifndef USE_PIEZO_SPEAKER
  // timer1 is used for tone
  Narcoleptic.disableTimer2();
  Narcoleptic.disableTimer1();
#endif
}

void waitTread() {

  long sleepTime = schedulerCnt.getTime();

  // if the main tread is waiting sleep for a while
#ifdef USE_SAVEPOWER
  Narcoleptic.delay(sleepTime);
#else
  delay(sleepTime);
#endif


#ifdef USE_SERIAL_DEBUG
  void writeSleepTimes();
#endif
}

