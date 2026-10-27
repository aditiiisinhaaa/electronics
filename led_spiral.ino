#include <LedControl.h>

const int DIN = 11;
const int CS = 10;
const int CLK = 13;

LedControl lc = LedControl(DIN, CLK, CS, 1);

const int animationDelay = 80;

const int outerSquare[][2] = {
  {0,0}, {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6}, {0,7},
  {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7},
  {7,6}, {7,5}, {7,4}, {7,3}, {7,2}, {7,1}, {7,0},
  {6,0}, {5,0}, {4,0}, {3,0}, {2,0}, {1,0}
};
const int outerSquareSize = 28;

const int innerSquare[][2] = {
  {2,2}, {2,3}, {2,4}, {2,5},
  {3,2}, {4,2}, {5,2},
  {5,3}, {5,4}, {5,5},
  {4,5}, {3,5}
};
const int innerSquareSize = 12;

int outerPos = 0;
int innerPos = 0;

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
}

void loop() {
  lc.clearDisplay(0);
  
  int outerRow = outerSquare[outerPos][0];
  int outerCol = outerSquare[outerPos][1];
  lc.setLed(0, outerRow, outerCol, true);
  
  int innerRow = innerSquare[innerPos][0];
  int innerCol = innerSquare[innerPos][1];
  lc.setLed(0, innerRow, innerCol, true);
  
  outerPos = (outerPos + 1) % outerSquareSize;
  innerPos = (innerPos + 1) % innerSquareSize;
  
  delay(animationDelay);
}