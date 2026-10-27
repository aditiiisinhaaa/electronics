#include<LedControl.h>
LedControl lc = LedControl(13, 11, 9, 1);
byte A[8] = { B00000000, B00111000, B01000100, B01000100, B01111100, B01000100, B01000100, B00000000 };
byte D[8] = { B00000000, B01111000, B01000100, B01000100, B01000100, B01000100, B01111000, B00000000 };
byte I[8] = { B00000000, B00111000, B00010000, B00010000, B00010000, B00010000, B00111000, B00000000 };
byte T[8] = { B00000000, B01111100, B00010000, B00010000, B00010000, B00010000, B00010000, B00000000 };
byte message[48];
int messageLength = 30;

void setup() {
  
  lc.shutdown(0, false); 
  lc.setIntensity(0, 8); 
  lc.clearDisplay(0); 

  
  int pos = 0;
  for (int i = 0; i < 8; i++) {
    message[pos++] = A[i];
    if (i < 7 || pos % 6 != 0) { 
      message[pos++] = A[i];
      message[pos++] = A[i];
      message[pos++] = A[i];
      message[pos++] = A[i];
    }
    message[pos++] = 0; 
  }
  for (int i = 0; i < 8; i++) {
    message[pos++] = D[i]; // Copy D
    if (i < 7 || pos % 6 != 0) {
      message[pos++] = D[i];
      message[pos++] = D[i];
      message[pos++] = D[i];
      message[pos++] = D[i];
    }
    message[pos++] = 0; // Gap column
  }
  for (int i = 0; i < 8; i++) {
    message[pos++] = I[i]; // Copy I (first)
    if (i < 7 || pos % 6 != 0) {
      message[pos++] = I[i];
      message[pos++] = I[i];
      message[pos++] = I[i];
      message[pos++] = I[i];
    }
    message[pos++] = 0; // Gap column
  }
  for (int i = 0; i < 8; i++) {
    message[pos++] = T[i]; // Copy T
    if (i < 7 || pos % 6 != 0) {
      message[pos++] = T[i];
      message[pos++] = T[i];
      message[pos++] = T[i];
      message[pos++] = T[i];
    }
    message[pos++] = 0; // Gap column
  }
  for (int i = 0; i < 8; i++) {
    message[pos++] = I[i]; // Copy I (second)
    if (i < 7 || pos % 6 != 0) {
      message[pos++] = I[i];
      message[pos++] = I[i];
      message[pos++] = I[i];
      message[pos++] = I[i];
    }
    message[pos++] = 0; // Gap column
  }
}

void loop() {
  for (int offset = 0; offset <= messageLength - 8; offset++) {
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, message[row + offset * 8]);
    }
    delay(100); // Adjust scrolling speed
  }
}