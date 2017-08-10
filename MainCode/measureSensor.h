// global measure values
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;
float valueDiff = 0;

// long integers for time measurement
long measureStart = 0;
long measureDuration = 0;

// parameter
const int sensorDifferenz = 50;

// filter variables and function
void calcFilter(float &value, int newValue, const int filterFaktor) {
  value = ((value * filterFaktor) + newValue) / (filterFaktor + 1.0);
}

// do capa measurement and calculations
void measureSensor() {
  // get start time
  measureStart = millis();

  // measure
  valueCurrent =  (float) capSensor.capacitiveSensor(10);

  // get measure duration
  measureDuration = millis() - measureStart;

  // store average value in first parameter
  calcFilter(valueAverage, valueCurrent, 5);
  calcFilter(valueSlow, valueCurrent, 50);
  valueDiff =  valueAverage - valueSlow;
}

