
// Vref = 1.1V => steps of 1.1/1024 => 1,07mV am Pin
// Imax = Vref/Rdown = 1.1V/10k = 0,11mA
// Vmax = Vref + Vup = Vref + Imax*Rup = 1.1V + 0,11*220K = 25.3 V // nano
// Vmax = Vref + Vup = Vref + Imax*Rup = 1.1V + 0,11*150K = 17.6 V // pro mini

// Bsp: analogRead(512) => 512/measurementFaktor = 12,66V

// measurement sensor pins
#ifdef USE_BREAKOUT_NANO
const float measurementFaktor = 1023/25.3;  // = 40.435,  
#else 
const float measurementFaktor = 1023/17.6;  // = 58.125,  
#endif

float voltageClamp15;
float voltageBCx;
int inputRawHandle;

void measureClamp15() {
  // read analog value
  int inputRawClamp15 = analogRead(pinInKl15);

  // calculate float value in volt
  voltageClamp15 = ((float) inputRawClamp15)/measurementFaktor;
}

void measureBCx()  {

  int inputRawBCx = analogRead(pinInBCx);

  // calculate float value in volt
  voltageBCx = ((float) inputRawBCx)/measurementFaktor;
}

void measureHandleTemp()  {
  // set to 5 volts for measuring voltage
  digitalWrite(pinMeasureHandleVcc, HIGH);

  // hopefully helps for better measurements
  delay(1);
  
  // read analog valuem, mosfets mast have low Ohms
  inputRawHandle = analogRead(pinInHandleTemp);

  // minus a fixed offset for a result with 2 Letters
  inputRawHandle -= 780;

  // sets measure Vcc pin back to LOW to make sure the diode locks
  digitalWrite(pinMeasureHandleVcc, LOW);

  /*
  Serial.print("inputRawHandle:");
  Serial.println(inputRawHandle, DEC);
  */
}

