#include <LedControl.h>

// Pin configuration for MAX7219 (DIN, CLK, CS, number of devices)
LedControl lc = LedControl(12, 11, 10, 1);

// Snowflake pattern (8x8 bitmap)
byte snowflake[8] = {
  B00100100,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B00100100
};

void setup() {
  // Initialize the MAX7219
  lc.shutdown(0, false); // Wake up display
  lc.setIntensity(0, 8); // Set brightness (0-15)
  lc.clearDisplay(0);    // Clear display
}

void loop() {
  // Display snowflake
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, snowflake[row]);
  }
  delay(500); // Show for 0.5 seconds
  
  // Twinkle effect (turn off briefly)
  lc.clearDisplay(0);
  delay(200); // Off for 0.2 seconds
}