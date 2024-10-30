// Written by K. M. Knausgård 2023-10-21

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

#include "mas245_logo_bitmap.h"

# define SCREEN_WIDTH 128
# define SCREEN_HEIGHT 64

namespace carrier
{
  namespace pin
  {
    constexpr uint8_t joyLeft{18};
    constexpr uint8_t joyRight{17};
    constexpr uint8_t joyClick{19};
    constexpr uint8_t joyUp{22};
    constexpr uint8_t joyDown{23};

    constexpr uint8_t oledDcPower{6};
    constexpr uint8_t oledCs{10};
    constexpr uint8_t oledReset{5};
  }

  namespace oled
  {
    constexpr uint8_t screenWidth{128}; // OLED display width in pixels
    constexpr uint8_t screenHeight{64}; // OLED display height in pixels
  }
}
namespace {
  CAN_message_t msg;

  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
  //FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

  Adafruit_SSD1306 display( carrier::oled::screenWidth,
                            carrier::oled::screenHeight,
                            &SPI,
                            carrier::pin::oledDcPower,
                            carrier::pin::oledReset,
                            carrier::pin::oledCs);
}

void setup() {
  Serial.begin(9600);
  int textx = 3;

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();

  int x = 0;
  int y = 0;

  int w = 128;
  int h = 64;
  int r = 5;

  display.drawRoundRect(x,y,w,h,r,SSD1306_WHITE);

  // Display Title
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 4); // Adjust position as needed
  display.print("MAS245 - Gruppe 7");

  // Draw line under the title
  int centerX = SCREEN_WIDTH / 2;   // Center X of the curve
  int yBase = 17;                   // Y position of the curve's lowest point
  int curveHeight = 6;             // Curve height (adjust for a steeper or gentler curve)
  int curveWidth = 128;             // Width of the curve (adjust to match line length)

  for (int x = -curveWidth / 2; x <= curveWidth / 2; x++) {
    int y = yBase - (curveHeight * x * x) / (curveWidth * curveWidth / 4); // Parabolic equation for a curve
    display.drawPixel(centerX + x, y, SSD1306_WHITE);
  }

  // Display CAN statistics label
  display.setCursor(textx, 20);
  display.print("CAN-statistikk");

    // Display dashed line (using shorter lines to create dash effect)
  for (int i = 1; i < 128; i += 3) {
    display.drawPixel(i, 30, SSD1306_WHITE);
    display.drawPixel(i + 1, 30, SSD1306_WHITE);
  }

  // Display statistics content
  display.setCursor(textx, 33);
  display.print("Antall mottatt: 31");

  display.setCursor(textx, 41);
  display.print("Mottok sist ID: 34");

  // Second dashed line
  for (int i = 1; i < 128; i += 3) {
    display.drawPixel(i, 51, SSD1306_WHITE);
    display.drawPixel(i + 1, 51, SSD1306_WHITE);
  }

  // Display IMU measurement
  display.setCursor(textx, 54);
  display.print("IMU z:  9.81m/s^2");

  display.display();
}



void loop() {

}

