// Written by K. M. Knausgård 2023-10-21

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

#include "mas245_logo_bitmap.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

namespace carrier {
namespace pin {
constexpr uint8_t joyLeft{ 18 };
constexpr uint8_t joyRight{ 17 };
constexpr uint8_t joyClick{ 19 };
constexpr uint8_t joyUp{ 22 };
constexpr uint8_t joyDown{ 23 };

constexpr uint8_t oledDcPower{ 6 };
constexpr uint8_t oledCs{ 10 };
constexpr uint8_t oledReset{ 5 };
}

namespace oled {
constexpr uint8_t screenWidth{ 128 };  // OLED display width in pixels
constexpr uint8_t screenHeight{ 64 };  // OLED display height in pixels
}
}


namespace images {
namespace pumpkin {
constexpr uint8_t width{ 16 };
constexpr uint8_t height{ 18 };

constexpr static uint8_t PROGMEM bitmap[] = {
  0b00000000,
  0b00100000,
  0b00000000,
  0b11100000,
  0b00000001,
  0b10000000,
  0b00000001,
  0b10000000,
  0b00000001,
  0b10000000,
  0b00000011,
  0b11100000,
  0b00001111,
  0b11111000,
  0b00111111,
  0b11111000,
  0b01111111,
  0b11111110,
  0b01111111,
  0b11111110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111111,
  0b11111110,
  0b01111111,
  0b11111110,
  0b00011111,
  0b11111000,
  0b00011111,
  0b11110000,
  0b00000111,
  0b11100000,
};
};
};


namespace {
CAN_message_t msg;

FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

Adafruit_SSD1306 display(carrier::oled::screenWidth,
                         carrier::oled::screenHeight,
                         &SPI,
                         carrier::pin::oledDcPower,
                         carrier::pin::oledReset,
                         carrier::pin::oledCs);
}


struct Message {
  uint8_t sequenceNumber;
  float temperature;
};


int textx = 3;

const int xPin = A0;  // X-akse er koblet til analog pin A0
const int yPin = A1;  // Y-akse er koblet til analog pin A1
const int zPin = A2;  // Z-akse er koblet til analog pin A2

void setup() {
  Serial.begin(9600);
  can0.begin();
  can0.setBaudRate(250000);



  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Stopp hvis skjermen ikke starter
  }

  display.clearDisplay();
  display.display();
  delay(500);
}


int motattMeldinger = 0;
void loop() {

  if (can0.read(msg)) {
    can0.write(msg);
  }
  readCan();  // Continuously check for incoming CAN messages
  sendCan();

  /*--------------------------------------------------------------------------------------------------------------------------------------------
                                                              Display settings
----------------------------------------------------------------------------------------------------------------------------------------------*/
  delay(1);
  //display.clearDisplay();

  int x = 0;
  int y = 0;

  int w = 128;
  int h = 64;
  int r = 5;

  display.drawRoundRect(x, y, w, h, r, SSD1306_WHITE);

  // Display Title
  display.setTextSize(1);  // Text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(16, 4);  // Adjust position as needed
  display.print("MAS245 - Gruppe 7");

  // Draw line under the title
  int centerX = SCREEN_WIDTH / 2;  // Center X of the curve
  int yBase = 17;                  // Y position of the curve's lowest point
  int curveHeight = 6;             // Curve height (adjust for a steeper or gentler curve)
  int curveWidth = 128;            // Width of the curve (adjust to match line length)

  for (int x = -curveWidth / 2; x <= curveWidth / 2; x++) {
    int y = yBase - (curveHeight * x * x) / (curveWidth * curveWidth / 4);  // Parabolic equation for a curve
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

  // Second dashed line
  for (int i = 1; i < 128; i += 3) {
    display.drawPixel(i, 51, SSD1306_WHITE);
    display.drawPixel(i + 1, 51, SSD1306_WHITE);
  }



  // Display statistics content
  display.setCursor(textx, 33);
  // // Clear previous number by drawing a black rectangle over it
  display.fillRect((128 - 35), 33, 34, 8, SSD1306_BLACK);  // Adjust size and position as needed
  display.print("Antall mottatt:");
  display.print(motattMeldinger);  // Antall medlinger via CANbus

  display.setCursor(textx, 41);
  display.fillRect((128 - 35), 41, 34, 8, SSD1306_BLACK);  // Adjust size and position as needed
  if (msg.id < 10)
    display.print("Mottok sist ID:00");
  else if (msg.id < 100)
    display.print("Mottok sist ID:0");
  else
    display.print("Mottok sist ID:");
  display.print(msg.id);  // ID via CANbus
  display.print("h");



  // Display IMU measurement
  display.setCursor(textx, 54);
  display.fillRect((128 - 35), 54, 34, 8, SSD1306_BLACK);  // Adjust size and position as needed
  display.print("IMU z:  ");
  display.print(1);  // IMU z via CANbus
  display.print(" m/s^2");

  //motattMeldinger++;

  display.display();
}

void sendCan() {
  msg.len = 7;
  msg.id = 0x007;  // Use HEX numbers
  can0.write(msg);
}

void readCan() {
  if (can0.read(msg)) {  // Check if a message is available and read it
    Serial.print("Received CAN message with ID: 0x");
    Serial.println(msg.id, HEX);
   

    // Print the data bytes of the message
    Serial.print("Data: ");
    for (int i = 0; i < msg.len; i++) {
      Serial.print(msg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Optionally, process the message data here
    // For example, you can store specific message IDs and data for further use
  } else {
    // No message available to read
  }
}

void readIMU () {
   // Les analoge verdier fra hver akse
  int xRaw = analogRead(xPin);
  int yRaw = analogRead(yPin);
  int zRaw = analogRead(zPin);

  // Konverterer råverdier til spenning (Teensy 3.6 bruker 3.3V referansespenning)
  float xVoltage = (xRaw / 1023.0) * 3.3;
  float yVoltage = (yRaw / 1023.0) * 3.3;
  float zVoltage = (zRaw / 1023.0) * 3.3;

  // ADXL335 har en skala på 0.3V/g (avhengig av spenning) rundt 1.65V ved 0g
  // For å få akselerasjonen i g-enheter
  float xAccel = (xVoltage - 1.65) / 0.3;  // Juster midtpunktet og del på 0.3V/g
  float yAccel = (yVoltage - 1.65) / 0.3;
  float zAccel = (zVoltage - 1.65) / 0.3;
}
