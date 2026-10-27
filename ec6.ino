#include "LedControl.h"

 //pin 12 is connected to the DataIn
// pin 11 is connected to the CLK
 //pin 10 is connected to LOAD

#define NBR_MTX 1 //number of matrices attached is one
LedControl lc=LedControl(12,11, 10, NBR_MTX);//
int i=0;
int j=0;
int l=0;
int m=0;
void setup()
{
  for (int i=0; i< NBR_MTX; i++)
  {
    lc.shutdown(i,false);
  /* Set the brightness to a medium values */
    lc.setIntensity(i,5);
  /* and clear the display */
    lc.clearDisplay(i);
    delay(500);
  }
}

void loop(){
  for(i,j;j<=7;j++){
    lc.setLed(0,i,0,true);
    lc.setLed(0,0,j,true);
    delay(50);
    lc.setLed(0,0,j,false);
    lc.setLed(0,i,0,false);
    i++;
  }
  i=1;
  j=1;
  for(i,j;j<=7;j++){
    lc.setLed(0,7,j,true);
    lc.setLed(0,i,7,true);
    delay(50);
    lc.setLed(0,7,j,false);
    lc.setLed(0,i,7,false);
    i++;
  }
  i=0;
  j=0;
}