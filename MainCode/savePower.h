
void initSavePower() {
  // Narcoleptic.disableMillis(); Do not disable millis - we need it for our delay() function.
  Narcoleptic.disableTimer1();
  Narcoleptic.disableTimer2();
  Narcoleptic.disableSerial();
  Narcoleptic.disableADC();
  Narcoleptic.disableWire();
  Narcoleptic.disableSPI();
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
  Serial.print(F("ms: "));
  Serial.print((millis() + Narcoleptic.millis()), DEC);
  Serial.print(F(" sleeped: "));
  Serial.println(sleepTime, DEC);
  // this delay is needed to be sure the text will be sent complete before sleeping with narcoleptic
  delay(2);
#endif
}

