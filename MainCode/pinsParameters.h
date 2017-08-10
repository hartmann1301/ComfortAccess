const byte pinLed = 13;

// measurement sensor pins
#ifdef USE_BREAKOUT_NANO
const byte pinSensorSend = 4;
const byte pinSensorReceive = 2;
#else 
const byte pinSensorSend = 2;
const byte pinSensorReceive = 4;
#endif

// measurement inputs pins
const byte pinInKl15 = A0;
const byte pinInBCx = A1;
const byte pinInHandleTemp = A7;
const byte pinInKillSwitch = 6;

const byte pinMeasureHandleVcc = 3;

// 0 = mosfet sperrt, 1 = mosfet zieht gegen GND und simuliert taster
const byte pinOutSlzPulldown = 10;

// 0 = heizgriff free floating für messung, 1 = heizgriff mit GND und BCx verbunden
const byte pinOutSetHeaterFree = 5;

// init sensor pin
CapacitiveSensor capSensor = CapacitiveSensor(pinSensorSend, pinSensorReceive);
MillisTimer schedulerCnt;
MillisTimer pressButtonCnt;
MillisTimer ignoreSensorCnt;
MillisTimer disableSensorCnt;

const int mainIntervalTime = 200;
const int ignoreAfterPushSim = 850;
const int igoreAfterKl15Off = 2050;
const int pressButtonTime = 350;
const float clamp15Treshold = 9.5;

boolean isClamp15Off;
boolean wasClamp15Off = false;
boolean isSlzPressSimActive = false;

void initPinsAndClassses() {
  pinMode(pinInKl15, INPUT);
  pinMode(pinInBCx, INPUT);
  pinMode(pinInKillSwitch, INPUT_PULLUP);

  pinMode(pinInHandleTemp, INPUT);  
  
#ifdef USE_HANDLETEMP
  // for measuring, this pin should be an output with 5 volts
  pinMode(pinMeasureHandleVcc, OUTPUT);
  digitalWrite(pinMeasureHandleVcc, LOW);
#else
  pinMode(pinMeasureHandleVcc, INPUT);
#endif

  pinMode(pinLed, OUTPUT);
  pinMode(pinOutSlzPulldown, OUTPUT);
  pinMode(pinOutSetHeaterFree, OUTPUT);

  // set outputs
  digitalWrite(pinOutSlzPulldown, LOW);

  digitalWrite(pinOutSetHeaterFree, HIGH);
  
  // init my counters
  schedulerCnt.resetTo(0);
  pressButtonCnt.resetTo(0);
  ignoreSensorCnt.resetTo(0); 
  disableSensorCnt.resetTo(0);

  // set Analog Reference Voltage to 1.1 volts
  analogReference(INTERNAL);

  // disable auto calibration, if measure time > mainIntervalTime set timeout
  capSensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
  //capSensor.set_CS_Timeout_Millis(CS_Timeout_Millis);
}

