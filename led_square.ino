#include <LedControl.h>
const int DIN = 11;
const int CS = 10;
const int CLK = 13;
LedControl lc= LedControl(DIN,CS,CLK,1);
void setup()
{ lc.shutdown(0,false);
lc.setIntensity(0,8);
lc.clearDisplay(0);
};
void loop()
{
  lc.clearDisplay(0);
  lc.setRow(0,0,255);
  lc.setColumn(0,7,255);
  lc.setRow(0,7,255);
  lc.setColumn(0,0,255);
}