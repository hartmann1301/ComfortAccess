#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OledGraph.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define GRAPH_WIDTH 64
#define LABELS_XPOS 90
#define LABELS_XUP  69

const int maxGraphValue = 10000;
const int maxVoltValue = 140;

OledGraph graphSensor = OledGraph(0, 0, GRAPH_WIDTH, 64 - 24, maxGraphValue);
OledGraph graphButton = OledGraph(0, 64 - (2 * 8 + 7), GRAPH_WIDTH, 7, 1);
OledGraph graphKl15   = OledGraph(0, 64 - (8 + 7), GRAPH_WIDTH, 7, maxVoltValue);
OledGraph graphBCx    = OledGraph(0, 64 - 7, GRAPH_WIDTH, 7, maxVoltValue);

void updateHand(byte posX, byte posY) {

  if (isHandDetected != wasHandDetected) {
    // update hand on display

    switch (isHandDetected) {
      case DETECTED_HAND:
        display.fillRect(posX, posY, 12, 12, BLACK);
        display.drawBitmap(posX, posY, logo_handIcon, 16, 12, WHITE);
        break;

      case NO_HAND:
        display.fillRect(posX, posY, 12, 12, BLACK);
        display.drawBitmap(posX, posY, logo_warnIcon, 16, 12, WHITE);
        break;

      case UNKNOWN_HAND:
        display.fillRect(posX, posY, 12, 12, BLACK);
        display.setCursor(posX + 2, posY + 2);
        display.print("?");
        break;

      default:
        break;
    }
    wasHandDetected = isHandDetected;
  }
}

void printGraphNames() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(LABELS_XUP, 8);
  display.print("Tm:");
  display.setCursor(LABELS_XUP, 16);
  display.print("lT:");
  display.setCursor(LABELS_XUP, 24);
  display.print("Ms:");

  display.setCursor(LABELS_XPOS, 64 - (2 * 8 + 7));
  display.print("Press");
  display.setCursor(LABELS_XPOS, 64 - (8 + 7));
  display.print("Kl15");
  display.setCursor(LABELS_XPOS, 64 - 7);
  display.print("BCx");

}

void initDisplay() {
  // set Pins for GND and 5v
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  // right line
  display.drawLine(127, 0, 127, 63, WHITE);

  printGraphNames();

  graphButton.drawScale();
  graphKl15.drawScale();
  graphBCx.drawScale();
  graphSensor.drawScale();

  // send changes to Display
  display.display();
}

void writeToDisplay() {
  graphButton.addLine(isSlzPressSimActive);
  graphKl15.addLine(voltageClamp15*10);
  graphBCx.addLine(voltageBCx*10);

  // if the sensor is ignored pirnt a line on top of the graph
  if (ignoreSensorCnt.getTime() != 0)
    graphSensor.addLine(maxGraphValue);

#ifdef USE_DETECTHAND
  // hands detecton Line in the graph
  graphSensor.addLine(handDetectThreshold);

  // print hand icon top right
  updateHand(128 - 13, 26);
#endif

  if (isClamp15Off) {
    graphSensor.addDots(valueSlow);
    graphSensor.addLine(valueAverage);
  } else {
    graphSensor.addDots(0);
  }

  // pirnt Sensor on or ignored Time 
  display.setCursor(LABELS_XUP, 0);  
  display.fillRect(LABELS_XUP, 0, 55, 8, BLACK);
  if (ignoreSensorCnt.getTime() == 0) {
    display.print("Sensor On");   
  } else {
    display.print("Ti:");  
    // write measureDuration value
    display.setCursor(LABELS_XUP + 20, 0);
    display.print(ignoreSensorCnt.getTime());
  }

  // clear space for diff value
  display.fillRect(LABELS_XUP + 20, 8, 20, 8, BLACK);
  // write measureDuration value
  display.setCursor(LABELS_XUP + 20, 8);
  display.print(measureDuration);

  // clear space for least total value
  display.fillRect(LABELS_XUP + 20, 16, 30, 8, BLACK);
  // write measureDuration value
  long tmpLeastTotal = capSensor.getLeastTotal();
  if (tmpLeastTotal >= 0x0FFFFFFFL)
    tmpLeastTotal = -1;
  display.setCursor(LABELS_XUP + 20, 16);
  display.print(tmpLeastTotal);

  // clear space samples
  display.fillRect(LABELS_XUP + 20, 24, 20, 8, BLACK);
  // write measureDuration value
  display.setCursor(LABELS_XUP + 20, 24);
  display.print(measureSamples); 

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

