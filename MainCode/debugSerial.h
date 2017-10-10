//float roun
void initSerial() {
  // start Serial debug
  Serial.begin(115200);
  Serial.println(F("Comfort Access Start"));
}

// serial printing
void writeToSerial() {  

  Serial.print((int) valueAverage);
  Serial.print("\t");
  Serial.print((int) valueDiff);
  Serial.print("\t");

  Serial.print("KL15:");
  Serial.println(round(voltageClamp15), DEC);

  /*
  graphButton.addLine(isSlzPressSimActive);  
  graphKl15.addLine(voltageClamp15);
  graphBCx.addLine(voltageBCx);
  */

  /*
  if (ignoreSensorCnt.getTime() == 0) {
    Serial.print("SensOn");
  } else {
    Serial.print("Ingore:");
  Serial.print(ignoreSensorCnt.getTime(), DEC);
  }
  Serial.print("\n");
  */
  
  #ifndef USE_SAVEPOWER
    delay(5);
  #endif
}

void writeSleepTimes() {
  /*
  Serial.print(F("ms: "));
  Serial.print((millis() + Narcoleptic.millis()), DEC);
  Serial.print(F(" sleeped: "));
  Serial.println(sleepTime, DEC);
  // this delay is needed to be sure the text will be sent complete before sleeping with narcoleptic
  delay(2);  
  */
}
