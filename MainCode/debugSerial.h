//float roun

// serial printing
void writeToSerial() {  
  Serial.print((int) valueAverage);
  Serial.print("\t");
  Serial.print((int) valueDiff);
  Serial.print("\t");

  Serial.print("KL15:");
  Serial.print(round(voltageClamp15), DEC);
  Serial.print("\t");

  /*
  graphButton.addLine(isSlzPressSimActive);  
  graphKl15.addLine(voltageClamp15);
  graphBCx.addLine(voltageBCx);
  */

  if (ignoreSensorCnt.getValue() == 0) {
    Serial.print("SensOn");
  } else {
    Serial.print("Ingore:");
  Serial.print(ignoreSensorCnt.getValue(), DEC);
  }
  Serial.print("\n");
}
