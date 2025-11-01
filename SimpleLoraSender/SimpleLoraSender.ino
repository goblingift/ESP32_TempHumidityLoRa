#include <RadioLib.h>

#define LED_EXTERN 21

SX1262 radio = new Module(41, 39, 42, 40); //support for the Wio_SX-1262 & Xiao ESP32S3 with B2B connector. 

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SX1262 with RadioLib");

  int status = radio.begin(868.0);  // Frequency 868 MHz for EU

  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true);
  }
  Serial.println("RadioLib init success");
}

void loop() {

  Serial.println("Sending message: Hello folks 2025");
  ledOn();
  // Sending a message
  Serial.println("Sending message...");
  String message = "Hello folks 2025";
  int state = radio.transmit(message);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.print("Failed to send, code: ");
    Serial.println(state);
  }

  ledOff();

  delay(10000);
}

void ledOn() {
    digitalWrite(LED_INTERN, HIGH);
    Serial.println("LED ON");
}

void ledOff() {
    digitalWrite(LED_INTERN, LOW);
    Serial.println("LED OFF");
}