#include <RadioLib.h>

#define LED_INTERN 21

SX1262 radio = new Module(41, 39, 42, 40); //support for the Wio_SX-1262 & Xiao ESP32S3 with B2B connector. 

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SX1262 LoRa Receiver with RadioLib");

  int status = radio.begin(868.0);  // Frequency 868 MHz for EU

  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true);
  }
  Serial.println("RadioLib init success");
}

void loop() {

  ledOn();
  receiveMessages();
  ledOff();

  Serial.println("Done with receiving messages- sleep!");
  delay(2000);
}

void ledOn() {
    digitalWrite(LED_INTERN, HIGH);
    Serial.println("LED ON");
}

void ledOff() {
    digitalWrite(LED_INTERN, LOW);
    Serial.println("LED OFF");
}


void receiveMessages() {

  Serial.println("Waiting for incoming messages...");
  uint8_t buf[256];  // Buffer for incoming message
  size_t len = sizeof(buf);
  int recvState = radio.receive(buf, len);

  if (recvState == RADIOLIB_ERR_NONE) {
    Serial.print("Received message: ");
    Serial.println((char*)buf);  // Cast to char* for printing
  } else if (recvState == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("Receive timeout, no message.");
  } else {
    Serial.print("Receive error, code: ");
    Serial.println(recvState);
  }


}