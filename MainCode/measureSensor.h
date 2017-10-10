// global measure values
float valueCurrent = 0;
float valueAverage = 0;
float valueSlow = 0;
float valueDiff = 0;

// long integers for time measurement
long measureStart = 0;
long measureDuration = 0;
long measureDurationSoll = 100;
long measureDurationDiff = 10;

const int calibrateOffset = 300;

int measureSamples = 60;
int sensorDifferenz = 400;

void initMeasureSensor() {
  // disable auto calibration, if measure time > mainIntervalTime set timeout
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);

  // stop at double of the main interval time to keep the programm running
  capSensor.set_CS_Timeout_Millis(mainIntervalTime*10);
}

// filter variables and function
void calcFilter(float &value, int newValue, const int filterFaktor) {
  value = ((value * filterFaktor) + newValue) / (filterFaktor + 1.0);
}

void changeSamples() {
  if (measureSamples == 0)
    return;

  // get time in ms pro sample
  float timeProSample = ((float) measureDuration) / measureSamples;  

  // calculate value of total pro sample
  long valueTotal = capSensor.getLeastTotal();
  float absolutTotal = valueCurrent + valueTotal;  
  float totalProSample = absolutTotal / measureSamples;

  /*
  Serial.print("old Samples : ");
  Serial.println(measureSamples, DEC);
  Serial.print("leastTotal  : ");
  Serial.println(valueTotal, DEC);
  Serial.print("valueCurrent: ");
  Serial.println(valueCurrent, DEC);
  Serial.print("messureTime : ");
  Serial.println(measureDuration, DEC);
  */

  // calculate how many samples are to be expected with new duration
  int tmpSamples = measureDurationSoll / timeProSample;
  int diffSamples = tmpSamples - measureSamples;

  // set new samples if valid
  if (tmpSamples < 1)
    tmpSamples = 1;

  if (tmpSamples > 900)
    tmpSamples = 900;

  measureSamples = tmpSamples;

  // set new least Total
  if (diffSamples > 0) {
    capSensor.setLeastTotal(valueTotal + totalProSample*diffSamples); 

    // maybe ignore next cycle
    ignoreSensorCnt.resetTo(mainIntervalTime); 
  }

  /*
  Serial.print("new Samples : ");
  Serial.println(measureSamples, DEC);
  Serial.print("us pro Sample : ");
  Serial.println((int) (timeProSample*1000), DEC);
  Serial.print("tot pro Sample: ");
  Serial.println((int) (totalProSample), DEC);
  Serial.print("TPS*diffSample: ");
  Serial.println((int) (totalProSample*diffSamples), DEC);
  Serial.println("");
  */
}

void calibrateTotal() {
  
    // get current value
  int vCorr = valueAverage;
  
  // some formula, should be used wisely because this kills diff
  vCorr /= calibrateOffset;
  vCorr *= vCorr;

  if (vCorr > calibrateOffset)
    vCorr = calibrateOffset;

  // set modified value back
  capSensor.setLeastTotal(capSensor.getLeastTotal() + vCorr);
}

// do capa measurement and calculations
void measureSensor() {

  measureStart = millis();

  // measure
  float tmpResult = (float) capSensor.capacitiveSensor(measureSamples);

  // plausi for measurement
  if (tmpResult >= 0 && tmpResult < 90000) {
    valueCurrent = tmpResult;
  } else {
    // set Errorflag for OLED
    errorFlag = true;

#ifdef USE_SERIAL_DEBUG
    Serial.print("unplausible measurement, received: ");
    Serial.println(valueCurrent, DEC);
#endif
  }

  // get measure duration
  measureDuration = millis() - measureStart;

  // store average value, first time direkt, than filtered
  if (valueAverage == 0) {
    //if (true) { // no filter!!!!!!!!!
    valueAverage = valueCurrent;
  } else {
    calcFilter(valueAverage, valueCurrent, 1);
  }

  // only for the first time
  if (valueSlow == 0) {
    valueSlow = valueAverage;
  } else {

    // slow value should be alays smaller than big value
    if (valueSlow > valueAverage + 20) {
      valueSlow = valueAverage;

    } else {
      calcFilter(valueSlow, valueCurrent, 20);
    }
  }

  valueDiff =  valueAverage - valueSlow;

  // try to stabilize the measuring time with slow parameter
  if (abs(measureDurationSoll - measureDuration) > measureDurationDiff)
    changeSamples();

  // try to stabilize temperature changes and decrease valueDiff
  calibrateTotal();
}

