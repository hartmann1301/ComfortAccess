// global measure values
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;
float valueDiff = 0;

// long integers for time measurement
long measureStart = 0;
long measureDuration = 0;

int measureSamples = 50;

// parameter
const int sensorDifferenz = 400;

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
}

