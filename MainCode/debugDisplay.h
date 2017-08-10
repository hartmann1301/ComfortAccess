#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <OledGraph.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define GRAPH_WIDTH 64
#define LABELS_XPOS 84

OledGraph graphSensor = OledGraph(0, 0, GRAPH_WIDTH, 64-27, 1500);
OledGraph graphKl15   = OledGraph(0, 64-(2*9+7), GRAPH_WIDTH, 7, 1);
OledGraph graphButton = OledGraph(0, 64-(9+7), GRAPH_WIDTH, 7, 1);
OledGraph graphBCO    = OledGraph(0, 64-7, GRAPH_WIDTH, 7, 1);

void printGraphNames() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(LABELS_XPOS, 0);
  display.print("Griff");
  display.setCursor(LABELS_XPOS, 8);
  display.print("Sensor");

  display.setCursor(LABELS_XPOS, 64-(2*9+7));
  display.print("Kl.15");

  display.setCursor(LABELS_XPOS, 64-(9+7));
  display.print("Button");

  display.setCursor(LABELS_XPOS, 64-7);
  display.print("BCO Vin");

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

  printGraphNames();

  graphKl15.drawScale();
  graphButton.drawScale();
  graphBCO.drawScale();
  graphSensor.drawScale();

  // send changes to Display
  display.display();
}

void updateDisplay() {
  graphKl15.addLine(activeClamp15);
  graphButton.addLine(activeKillSwitch);
  graphBCO.addLine(0);
  
  graphSensor.addDots(valueSlow);
  graphSensor.addLine(valueAverage);

  // tick the whole left side one pixel left
  graphKl15.tickRect(0, 0, GRAPH_WIDTH, 64);

  // send changes to Display
  display.display();
}

