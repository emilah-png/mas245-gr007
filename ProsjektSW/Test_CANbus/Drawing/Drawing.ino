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
CAN_message_t imuMsg;
CAN_message_t startMsg;
CAN_message_t svarMsg;



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
const int zPin = A7;  // Z-akse
int motattMeldinger = 0;

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
  delay(1000);
  sendStartMessage();
}




/*--------------------------------------------------------------------------------------------------------------------------------------------
                                                              CANbus settings
----------------------------------------------------------------------------------------------------------------------------------------------*/
/*
void sendCan() {
  msg.len = 1;
  msg.id = imuMsg.id;  // Use HEX numbers
  can0.write(msg);
}*/


void readCan() {
  if (can0.read(imuMsg)) {  // Check if a message is available and read it
    Serial.print("Received CAN message with ID: 0x");
    Serial.println(imuMsg.id, HEX);

    motattMeldinger++;

    // Print the data bytes of the message
    Serial.print("Data: ");
    for (int i = 0; i < imuMsg.len; i++) {
      Serial.print(imuMsg.buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Optionally, process the message data here
    // For example, you can store specific message IDs and data for further use
  } else {
    // No message available to read
  }
}

void sendStartMessage() {

  startMsg.id = 0x021;  // Angir meldings-ID
  startMsg.len = 1;     // Velg lengden på data, for eksempel 1 byte

  // Sett opp innholdet i meldingen, f.eks. en byte med 0x01 for å indikere "start"
  startMsg.buf[0] = 0x00;
  can0.write(startMsg);
  delay(5000);
  startMsg.buf[0] = 0x01;
  can0.write(startMsg);
}

void sendIMUMessage(uint16_t value) {

  imuMsg.id = 0x022;  // Angir meldings-ID
  imuMsg.len = 2;     // Velg lengden på data, for eksempel 1 byte

  // Sett opp innholdet i meldingen, f.eks. en byte med 0x01 for å indikere "start"
  imuMsg.buf[0] = (value >> 8) & 0xFF;
  imuMsg.buf[1] = value & 0xFF;
  // Send meldingen
  can0.write(imuMsg);
}

void loop() {

  int zRaw = analogRead(zPin);

  sendIMUMessage(zRaw);
  readCan();  // Continuously check for incoming CAN messages
  //sendCan();
  delay(1000);

  

    /*--------------------------------------------------------------------------------------------------------------------------------------------
                                                              Display settings
----------------------------------------------------------------------------------------------------------------------------------------------*/

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
    if (imuMsg.id == 0) {
      display.print("Ingen mottatt ID");
    } else {
      if (imuMsg.id < 10) {
        display.print("Mottok sist ID:0x00");
      } else if (imuMsg.id < 100) {
        display.print("Mottok sist ID:0x0");
      } else {
        display.print("Mottok sist ID:0x");
      }
    }
    display.print(imuMsg.id, HEX);  // ID via CANbus

    svarMsg.id = imuMsg.id;
    svarMsg.len = imuMsg.len;

    for (int i = 0; i < svarMsg.len; i++) {
      imuMsg.buf[i] = svarMsg.buf[i];
    }


    display.setCursor(textx, 54);
    display.fillRect((128 - 35), 54, 34, 8, SSD1306_BLACK);  // Adjust size and position as needed
    display.print("IMU z:");
    //display.print(print(imuMsg));  // IMU z via CANbus
    display.print("m/s^2");

    
  
  display.display();
}



/*--------------------------------------------------------------------------------------------------------------------------------------------
                                                              IMU readings
----------------------------------------------------------------------------------------------------------------------------------------------*/
/*void readIMU () {

  // Pinout per akse
  const int xPin = A9;  // X-akse
  const int yPin = A8;  // Y-akse
  const int zPin = A7;  // Z-akse

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
  float xAccel = 9.81 * (xVoltage-(3.3/2)) / 0.3;  // Juster midtpunktet og del på 0.3V/g
  float yAccel = 9.81 * (yVoltage-(3.3/2)) / 0.3;
  float zAccel = 9.81 * (zVoltage-(3.3/2)) / 0.3;

  float totalAcc = sqrt(xAccel*xAccel + yAccel*yAccel + zAccel*zAccel);


  Serial.print("X: ");
  Serial.print(xVoltage);
  Serial.print(" V, Y: ");
  Serial.print(yVoltage);
  Serial.print(" V, Z: ");
  Serial.print(zVoltage);
  Serial.print(" V | ");
  Serial.print("X: ");
  Serial.print(xAccel);
  Serial.print(" m/s, Y: ");
  Serial.print(yAccel);
  Serial.print(" m/s, Z: ");
  Serial.print(zAccel);
  Serial.print(" m/s | ");
  Serial.print(totalAcc);
  Serial.println(" m/s");
}*/




/*
void sendUint16(uint16_t value) {
  msg.id = 0x021;  // Sett meldings-ID
  msg.len = 2;     // To byte for uint16_t verdi

  // Del opp uint16_t i to byte og legg dem i meldingsbufferen
  msg.buf[0] = (value >> 8) & 0xFF;  // Mest signifikante byte (MSB)
  msg.buf[1] = value & 0xFF;         // Minst signifikante byte (LSB)

  // Send meldingen
  if (can0.write(msg)) {
    Serial.println("uint16_t verdi sendt over CAN");
  } else {
    Serial.println("Feil ved sending av CAN-melding");
  }
}*/
