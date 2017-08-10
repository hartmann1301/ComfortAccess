// global measure values
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;
float valueDiff = 0;

// long integers for time measurement
int measureStart = 0;
int measureDuration = 0;

const byte measureDurationSoll = 120;
const byte measureDurationDiff = 10;

int measureSamples = 50;

// parameter
const int sensorDifferenz = 2000;

void initMeasureSensor() {
  // disable auto calibration, if measure time > mainIntervalTime set timeout
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);

  // stop at double of the main interval time to keep the programm running
  capSensor.set_CS_Timeout_Millis(mainIntervalTime * 2);
}


// filter variables and function
void calcFilter(float &value, int newValue, const int filterFaktor) {
  value = ((value * filterFaktor) + newValue) / (filterFaktor + 1.0);
}

// do capa measurement and calculations
void measureSensor() {
  // get start time, narcoleptic is not needed because the uc can not sleep while measuring
  measureStart = millis();

  // measure
  valueCurrent =  (float) capSensor.capacitiveSensor(measureSamples);

  // get measure duration
  measureDuration = millis() - measureStart;

  // store average value in first parameter
  calcFilter(valueAverage, valueCurrent, 3);

  // if the sensor is disabled is measures, but there will be no diff
  if (disableSensorCnt.getTime() == 0) {
    calcFilter(valueSlow, valueCurrent, 20);
  } else {
    valueSlow = valueAverage;
  }

  valueDiff =  valueAverage - valueSlow;

  // try to stabilize the measuring time with slow parameter
  if (measureDuration < (measureDurationSoll - measureDurationDiff)) {
    // measure longer to get better results

    // dangerous because the valueCurrent/valueAverage will grow, so maybe increase valueSlow a bit
    measureSamples++;
  } else if (measureDuration > (measureDurationSoll + measureDurationDiff)) {
    // took too long measure less
    measureSamples--;
  }

  // try to autocalibrate if the measurement is quiet
  if (valueDiff < sensorDifferenz / 2) {
    // get current value
    unsigned long tmpTotal = capSensor.getLeastTotal();

    // some formula
    if (valueCurrent > 1000)
      tmpTotal += valueCurrent / 200;

    // set modified value back
    capSensor.setLeastTotal(tmpTotal);
  }
}

