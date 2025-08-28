/**
 * Gear Mapping and Display using Arduino UNO R3
 * ---------------------------------------------
 * Reads four analog Hall sensors (A0–A3) to determine gearbox position:
 *   R, N, 1, 2, 3, 4, 5
 * Displays corresponding bitmaps on a 128x64 SSD1306 OLED via U8g2.
 *
 * MODE pin (D3): 0 = debug layout (small icon at right), 1 = fullscreen icon.
 *
 * Notes:
 * - Thresholding uses "percent deviation from 512" for noise robustness.
 * - Bitmap index mapping uses (current_gear + 1):
 *     current_gear = -1 -> index 0 (R)
 *     current_gear =  0 -> index 1 (N)
 *     current_gear =  1..5 -> index 2..6
 *
 * Hardware (UNO R3):
 *   Hall0 -> A0, Hall1 -> A1, Hall2 -> A2, Hall3 -> A3
 *   OLED  -> SDA=A4, SCL=A5 (I2C)
 *   MODE  -> D3 (use external pull-up or pull-down to avoid floating)
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "gear_bitmaps_fullscreen.h"

// OLED: SSD1306 128x64, I2C, full buffer mode
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /*reset=*/U8X8_PIN_NONE);

// -------- Pins --------
static const uint8_t PIN_HALL_0 = A0;
static const uint8_t PIN_HALL_1 = A1;
static const uint8_t PIN_HALL_2 = A2;
static const uint8_t PIN_HALL_3 = A3;
static const uint8_t PIN_MODE   = 3;   // LOW=debug, HIGH=fullscreen

// -------- Tuning --------
static const float   ADC_CENTER          = 512.0f; // mid of 10-bit ADC
static const float   PERCENT_DIVISOR     = 5.12f;  // 1024 / 100
static const uint8_t THRESHOLD_PERCENT   = 30;     // trip threshold (adjust for your sensors)

// Current gear state; -1=R, 0=N, 1..5
int current_gear = 0;

void setup() {
  pinMode(PIN_HALL_0, INPUT);
  pinMode(PIN_HALL_1, INPUT);
  pinMode(PIN_HALL_2, INPUT);
  pinMode(PIN_HALL_3, INPUT);
  pinMode(PIN_MODE,   INPUT); // add external pull-up/down to avoid floating

  u8g2.begin();
}

void loop() {
  // ---- Read sensors ----
  int hall0 = analogRead(PIN_HALL_0);
  int hall1 = analogRead(PIN_HALL_1);
  int hall2 = analogRead(PIN_HALL_2);
  int hall3 = analogRead(PIN_HALL_3);

  // Convert to percentage deviation from center (512)
  int p0 = round(abs(hall0 - ADC_CENTER) / PERCENT_DIVISOR);
  int p1 = round(abs(hall1 - ADC_CENTER) / PERCENT_DIVISOR);
  int p2 = round(abs(hall2 - ADC_CENTER) / PERCENT_DIVISOR);
  int p3 = round(abs(hall3 - ADC_CENTER) / PERCENT_DIVISOR);

  // ---- Decide gear from simple rules ----
  if (p0 > THRESHOLD_PERCENT && p3 > THRESHOLD_PERCENT) {
    current_gear = 3; // 3rd gear
  } else if (p1 > THRESHOLD_PERCENT && p2 > THRESHOLD_PERCENT) {
    current_gear = 4; // 4th gear
  } else if (p0 > THRESHOLD_PERCENT) {
    current_gear = 1; // 1st gear
  } else if (p3 > THRESHOLD_PERCENT) {
    current_gear = 5; // 5th gear
  } else if (p1 > THRESHOLD_PERCENT) {
    current_gear = 2; // 2nd gear
  } else if (p2 > THRESHOLD_PERCENT) {
    current_gear = -1; // reverse
  } else {
    current_gear = 0; // neutral
  }

  // ---- Display ----
  int mode = digitalRead(PIN_MODE); // 0=debug, 1=fullscreen
  u8g2.clearBuffer();

  // Map gear to bitmap index
  uint8_t bmpIndex = (uint8_t)(current_gear + 1);
  // Safety clamp
  if (bmpIndex > 6) bmpIndex = 1; // default to Neutral on unexpected value

  if (mode == LOW) {
    // Debug layout: draw icon at right side (64..127)
    u8g2.drawXBMP(64, 0, 64, 64, bitmaps_gears_fullscreen[bmpIndex]);
  } else {
    // Fullscreen icon centered
    u8g2.drawXBMP(28, 0, 72, 64, bitmaps_gears_fullscreen[bmpIndex]);
  }

  u8g2.sendBuffer();

  // (Optional) small delay to reduce flicker; uncomment if needed
  // delay(20);
}
