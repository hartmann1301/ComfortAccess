
// Vref = 1.1V
// Imax = Vref/Rdown = 1.1V/10k = 0,11mA
// Vmax = Vref + Vup = Vref + Imax*Rup = 1.1V + 0,11*220K = 25.3 V

// Bsp: analogRead(512) => 512/measurementFaktor = 12,66V

const float measurementFaktor = 1023/25.3;  // 1023/25.3 = 40.435,  

int inputRawClamp15;
int inputRawBCx;

float voltageClamp15;
float voltageBCx;

void measureClamp15() {
  // read analog value
  inputRawClamp15 = analogRead(pinInKl15);

  // calculate float value in volt
  voltageClamp15 = ((float) inputRawClamp15)/measurementFaktor;
}

void measureBCx()  {

  inputRawBCx = analogRead(pinInBCx);

  // calculate float value in volt
  voltageBCx = ((float) inputRawBCx)/measurementFaktor;
}

