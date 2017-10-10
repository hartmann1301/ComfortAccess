#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OledGraph.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define GRAPH_WIDTH 64
#define GRAPH_HEIGTH 40
#define GRAPH_LABELS_X 90

#define VARS_LABELS_LX  69
#define VARS_VALUE_LX VARS_LABELS_LX + 10
#define VARS_LABELS_RX 99
#define VARS_VALUE_RX VARS_LABELS_RX + 10

#define DIGITS_2 2*5 + 1
#define DIGITS_3 3*5 + 2
#define DIGITS_4 4*5 + 3
#define DIGITS_5 5*5 + 4

#define LINE0 0
#define LINE1 1*8
#define LINE2 2*8
#define LINE3 3*8
#define LINE4 4*8 + 1
#define LINE5 5*8 + 1
#define LINE6 6*8 + 1
#define LINE7 7*8 + 1

const int maxGraphValue = 2000;
const int maxVoltValue = 140;

#define GRAPH_RASTER maxGraphValue/GRAPH_HEIGTH

OledGraph graphSensor = OledGraph(0, 0, GRAPH_WIDTH, GRAPH_HEIGTH, maxGraphValue);
OledGraph graphButton = OledGraph(0, 64 - (2 * 8 + 7), GRAPH_WIDTH, 7, 1);
OledGraph graphKl15   = OledGraph(0, 64 - (8 + 7), GRAPH_WIDTH, 7, maxVoltValue);
OledGraph graphBCx    = OledGraph(0, 64 - 7, GRAPH_WIDTH, 7, maxVoltValue);

void printGraphNames() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(VARS_LABELS_LX, LINE0);
  display.print("S");
  display.setCursor(VARS_LABELS_LX + 4, LINE0);
  display.print(":");

  display.setCursor(VARS_LABELS_LX, LINE1);
  display.print("T");
  display.setCursor(VARS_LABELS_LX + 4, LINE1);
  display.print(":99999");

  display.setCursor(VARS_LABELS_LX, LINE2);
  display.print("t");
  display.setCursor(VARS_LABELS_LX + 4, LINE2);
  display.print(":999");

  display.setCursor(VARS_LABELS_RX, LINE2);
  display.print("s");
  display.setCursor(VARS_LABELS_RX + 4, LINE2);
  display.print(":999");

  /*
    display.setCursor(VARS_LABELS_RX, LINE3);
    display.print("r");
    display.setCursor(VARS_LABELS_RX + 4, LINE3);
    display.print(":999");
  */

  display.setCursor(GRAPH_LABELS_X, LINE5);
  display.print("Pr");
  display.setCursor(GRAPH_LABELS_X + 10, LINE5);
  display.print(":");

  display.setCursor(GRAPH_LABELS_X, LINE6);
  display.print("Kl15");
  display.setCursor(GRAPH_LABELS_X, LINE7);
  display.print("V BCx");
}

void prepareToPrint(const byte xPos, const byte yPos, const byte clearSpace) {
  display.fillRect(xPos, yPos, clearSpace, 8, BLACK);
  display.setCursor(xPos, yPos);
}

void verifyAndPrint(const long value, const long minium, const long maximum ) {
  if (value >= minium && value <= maximum) {
    display.print(value, DEC);
  } else {
    display.print(F("OV"));
  }
}

void initDisplay() {
  // set Pins for GND and 5v
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  /*
   * Startup Begin
   */
  display.display();
  
#ifdef USE_SAVEPOWER
  Narcoleptic.delay(2000);
#else
  delay(2000);
#endif
  
  display.clearDisplay();
  display.drawBitmap(0, 0, startscreen, 128, 64, 1);  
 
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, LINE6);
  display.print(F("v: 1.15"));

  display.setCursor(0, LINE7);
  display.print(F("27.09.17"));

  display.display();
  
#ifdef USE_SAVEPOWER
  Narcoleptic.delay(5000);
#else
  delay(5000);
#endif

  /*
   * Startup End
   */

  display.clearDisplay();

  display.setCursor(24, 4);
  display.print(F("start"));
  display.setCursor(20, 12);
  display.print(F("measure"));
  
  printGraphNames();

  graphButton.drawScale();
  graphKl15.drawScale();
  graphBCx.drawScale();
  graphSensor.drawScale();

  // send changes to Display
  display.display();
}

void printTime(byte xPos, byte yPos, long mil) {
  // change to seconds
  int secondsCounter = mil / 1000;

  byte timeSeconds = secondsCounter % 60;
  byte timeMinutes = secondsCounter / 60;
  byte timeHours = secondsCounter / 3600;

  char strBuffer[3];

  display.setCursor(xPos + 10, LINE0);
  display.print(":");
  display.setCursor(xPos + 24, LINE0);
  display.print(":");

  sprintf(strBuffer, "%02d", timeHours);
  prepareToPrint(xPos, yPos, DIGITS_2);
  display.print(strBuffer);

  sprintf(strBuffer, "%02d", timeMinutes);
  prepareToPrint(xPos + 14, yPos, DIGITS_2);
  display.print(strBuffer);

  sprintf(strBuffer, "%02d", timeSeconds);
  prepareToPrint(xPos + 28, yPos, DIGITS_2);
  display.print(strBuffer);
}

void writeToDisplay() {
  graphButton.addLine(isSlzPressSimActive);
  graphKl15.addLine(voltageClamp15 * 10);
  graphBCx.addLine(voltageBCx * 10);

  // if the sensor is ignored pirnt a line on top of the graph
  if (ignoreSensorCnt.getTime() != 0)
    graphSensor.addLine(maxGraphValue);

  // if the sensor is disabled pirnt a line on top of the graph
  if (disableSensorCnt.getTime() != 0)
    graphSensor.addLine(GRAPH_RASTER * 38);


  // check the error Flag, show and reset if set
  if (errorFlag) {
    graphSensor.addLine(maxGraphValue);

    graphSensor.addLine(GRAPH_RASTER * 30);
    graphSensor.addLine(GRAPH_RASTER * 32);
    graphSensor.addLine(GRAPH_RASTER * 33);
    graphSensor.addLine(GRAPH_RASTER * 34);

    errorFlag = false;
  }

  if (isClamp15Off) {
    graphSensor.addDots(valueSlow);
    graphSensor.addLine(valueAverage);
  } else {
    graphSensor.addDots(0);
  }

  // time since kl15 off
  printTime(VARS_VALUE_LX, LINE0, kl15OffCounter);

  //least total value
  prepareToPrint(VARS_VALUE_LX, LINE1, DIGITS_5);
  verifyAndPrint(capSensor.getLeastTotal(), 0 , 99999);

  // measurement duration
  prepareToPrint(VARS_VALUE_LX, LINE2, DIGITS_3);
  verifyAndPrint(measureDuration, 0 , 900);

  // measurement samples
  prepareToPrint(VARS_VALUE_RX, LINE2, DIGITS_3);
  verifyAndPrint(measureSamples, 1, 900);

  // mode
  prepareToPrint(VARS_LABELS_LX, LINE3, DIGITS_5);
  if (isClamp15Off) {
    if (measureMode == AWAKE) {
      display.print(F("Awake"));
    } else {
      display.print(F("Sleep"));
    }
  } else {
    display.print(F("Ig.On"));
  }

  /* rauschen of measurement
  prepareToPrint(VARS_VALUE_RX, LINE3, DIGITS_4);
  verifyAndPrint(valueRauschen, 0, 999);
  */

  // senosr diff
  prepareToPrint(VARS_LABELS_RX, LINE4, DIGITS_4);
  verifyAndPrint(valueDiff, -999, 9999);

  // press counter
  prepareToPrint(VARS_VALUE_RX, LINE5, DIGITS_3);
  verifyAndPrint(pressSlzCounter, 0, 999);

#ifdef USE_HANDLETEMP
  // print measured handle temperature
  display.setCursor(127 - 13, 33);
  display.fillRect(127 - 13, 33, 12, 8, BLACK);
  if (inputRawHandle > 0 && inputRawHandle < 100) {
    display.print(inputRawHandle);
  } else {
    display.print("R?");
  }
#endif

  // tick the whole left side one pixel left
  graphKl15.tickRect(0, 0, GRAPH_WIDTH, 64);

  // send changes to Display
  display.display();
}
