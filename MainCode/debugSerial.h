// serial printing
void printSerial() {  
  Serial.print((int) valueAverage);
  Serial.print("\t");
  Serial.print((int) valueDiff);
  Serial.print("\t");

  Serial.print("Kl.15:");
  Serial.print(activeClamp15);
  Serial.print("\t");

  if (ignoreSensorCnt.getValue() == 0) {
    Serial.print("SensOn");
  } else {
    Serial.print("Ingore:" + String(ignoreSensorCnt.getValue()));
  }
  Serial.print("\n");
}
