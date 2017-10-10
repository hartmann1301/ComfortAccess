/* Measurement

  // Vref = 1.1V => steps of 1.1/1024 => 1,07mV am Pin
  // Imax = Vref/Rdown = 1.1V/10k = 0,11mA
  // Vmax = Vref + Vup = Vref + Imax*Rup = 1.1V + 0,11*220K = 25.3 V // nano
  // Vmax = Vref + Vup = Vref + Imax*Rup = 1.1V + 0,11*150K = 17.6 V // pro mini

  // Bsp: analogRead(512) => 512/measurementFaktor = 12,66V

  #ifdef USE_BREAKOUT_NANO
  const float measurementFaktor = 1023 / 25.3; // = 40.435,
  #endif
*/

const float measurementFaktor = 1023 / 17.6; // = 58.125,

float voltageClamp15;
float voltageBCx;
float lastVoltageBCx;
int inputHandleRaw;

float getVoltage(byte measurePin) {
  // read analog value and correct it with faktor from voltage divider
  return ((float) analogRead(measurePin)) / measurementFaktor;
}

void measureClamp15() {
  // save current voltage global
  voltageClamp15 = getVoltage(pinInKl15);
}

void measureBCx()  {
  // save last voltage for flag detection
  lastVoltageBCx = voltageBCx;

  // save current voltage global
  voltageBCx = getVoltage(pinInBCx);
}

void checkBCxSwitchOff() {
  if (lastVoltageBCx > 0.6 && voltageBCx < 0.4) {
    setModeSleep();
  }

  /* go to sleep because bcx is off
    if (voltageBCx < 0.1) {
    mainIntervalTime = mainIntervalTimeSleep;
    }
  */
}

void measureHandleTemp()  {
  // set to 5 volts for measuring voltage
  digitalWrite(pinMeasureHandleVcc, HIGH);

  // hopefully helps for better measurements
  delay(1);

  // read analog valuem, mosfets mast have low Ohms
  inputHandleRaw = analogRead(pinInHandleTemp);

  // minus a fixed offset for a result with 2 Letters
  inputHandleRaw -= 780;

  // sets measure Vcc pin back to LOW to make sure the diode locks
  digitalWrite(pinMeasureHandleVcc, LOW);

  /*
    Serial.print("inputHandleRaw:");
    Serial.println(inputHandleRaw, DEC);
  */
}

