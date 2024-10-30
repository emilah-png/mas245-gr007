// Written by K. M. Knausgård 2023-10-21

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

#include "mas245_logo_bitmap.h"

namespace {
  CAN_message_t msg;
  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
}

void setup() {
  Serial.begin(9600);
  can0.begin();
  can0.setBaudRate(250000);

}

void sendCan(){
  msg.len = 1;
  msg.id = 0x021;
  if (can0.write(msg)) {
    Serial.println("Message sent successfully.");
  } else {
    Serial.println("CAN send failed.");
  }
}

void readCan() {
  // Check if a CAN message is available and read it
  if (can0.read(msg)) {  
    Serial.print("Received CAN message with ID: 0x");
    Serial.print(msg.id, HEX);  // Print message ID in hexadecimal

    Serial.print(", Length: ");
    Serial.print(msg.len);  // Print the length of the message
    
    Serial.print(", Data: ");
    for (int i = 0; i < msg.len; i++) {
      Serial.print("0x");
      Serial.print(msg.buf[i], HEX);  // Print each data byte in hexadecimal
      Serial.print(" ");
    }
    Serial.println();  // Newline for the next message
  }
}

void loop() {

  readCan();

  sendCan();  

  delay(100);
}


