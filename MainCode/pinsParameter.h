const byte pinLed = 13;

// measurement sensor pins
#ifdef USE_BREAKOUT_NANO
const byte pinSensorSend = 4;
const byte pinSensorReceive = 2;
#else
const byte pinSensorSend = 2;
const byte pinSensorReceive = 7;
#endif

// measurement inputs pins
const byte pinInKl15 = A0;
const byte pinInBCx = A1;
const byte pinInHandleTemp = A7;
const byte pinInKillSwitch = 6;

const byte pinMeasureHandleVcc = 3;

const byte pinSpeakerOut = 8;

// 0 = mosfet sperrt, 1 = mosfet zieht gegen GND und simuliert taster
const byte pinOutSlzPulldown = 10;

// 0 = heizgriff free floating für messung, 1 = heizgriff mit GND und BCx verbunden
const byte pinOutSetHeaterFree = 5;

// init sensor pin
CapacitiveSensor capSensor = CapacitiveSensor(pinSensorSend, pinSensorReceive);
MillisTimer startUpCnt;
MillisTimer schedulerCnt;
MillisTimer pressButtonCnt;
MillisTimer ignoreSensorCnt;
MillisTimer disableSensorCnt;
MillisTimer resetDetectionCnt;

const int mainIntervalTimeAwake = 150;
const int mainIntervalTimeSleep = 250;
int mainIntervalTime = mainIntervalTimeAwake;

enum {AWAKE, SLEEP};
byte measureMode = AWAKE;

void setModeAwake() {
  mainIntervalTime = mainIntervalTimeAwake;

  // only for the disp
  measureMode = AWAKE;
}

void setModeSleep() {
  mainIntervalTime = mainIntervalTimeSleep;

  // only for the disp
  measureMode = SLEEP;
}

const int ignoreAfterPushSim = 850;
const int igoreAfterKl15Off = 2050;
const int pressButtonTime = 350;
const float clamp15Treshold = 9.5;

boolean isClamp15Off;
boolean wasClamp15Off = false;
boolean isSlzPressSimActive = false;
boolean errorFlag = true;

int pressSlzCounter = 0;
long kl15OffCounter = 0;
long kl15OffOffset = 0;

void initPinsAndClassses() {
  pinMode(pinInKl15, INPUT);
  pinMode(pinInBCx, INPUT);
  pinMode(pinInKillSwitch, INPUT_PULLUP);

  pinMode(pinInHandleTemp, INPUT);

  // for measuring, this pin should be an output with 5 volts
  pinMode(pinMeasureHandleVcc, OUTPUT);
  digitalWrite(pinMeasureHandleVcc, LOW);

  pinMode(pinLed, OUTPUT);
  pinMode(pinOutSlzPulldown, OUTPUT);
  pinMode(pinOutSetHeaterFree, OUTPUT);

  // set outputs
  digitalWrite(pinOutSlzPulldown, LOW);
  digitalWrite(pinOutSetHeaterFree, LOW);

  // init my counters
  startUpCnt.resetTo(1000);
  schedulerCnt.resetTo(0);
  pressButtonCnt.resetTo(0);
  ignoreSensorCnt.resetTo(1000);

  // set Analog Reference Voltage to 1.1 volts
  analogReference(INTERNAL);
}
