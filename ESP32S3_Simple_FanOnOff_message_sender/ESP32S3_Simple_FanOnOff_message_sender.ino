#include "Wire.h"
#include <RadioLib.h>

#define LED_BUILTIN 21

SX1262 radio = new Module(41, 39, 42, 40);  // Wio SX-1262 & XIAO ESP32S3

// 0 = will send FAN:ON next, 1 = will send FAN:OFF next
bool fanOnNext = true;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting XIAO ESP32S3 FAN command sender!");
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Starting SX1262 LoRa Sender!");
  int status = radio.begin(868.0);  // 868 MHz EU
  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true) { }
  }
  Serial.println("RadioLib init success");
}

void loop() {
  ledOn();

  // Choose message based on current state
  String msg;
  if (fanOnNext) {
    msg = "FAN:ON";
  } else {
    msg = "FAN:OFF";
  }

  // Send LoRa message
  int state = radio.startTransmit(msg);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Message sent successfully: ");
    Serial.println(msg);
  } else {
    Serial.print("Failed to send, code: ");
    Serial.println(state);
  }

  ledOff();

  // Toggle for next round
  fanOnNext = !fanOnNext;

  // Wait 15 seconds before sending the next command
  delay(15000);
}

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);   // internal LED is reversed on XIAO ESP32S3
  Serial.println("LED ON");
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED OFF");
}
