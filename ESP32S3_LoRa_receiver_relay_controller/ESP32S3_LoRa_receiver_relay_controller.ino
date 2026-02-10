#include <RadioLib.h>

#define LED_INTERN 21      // Onboard LED on XIAO ESP32S3
#define LED_EXTERN 1
#define RELAY 2

SX1262 radio = new Module(41, 39, 42, 40);  // Wio SX1262 + XIAO ESP32S3 B2B

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SX1262 LoRa Receiver with RadioLib");

  pinMode(LED_INTERN, OUTPUT);
  pinMode(LED_EXTERN, OUTPUT);
  pinMode(RELAY, OUTPUT);

  // Start with everything off
  deactivateRelay();

  int status = radio.begin(868.0);  // 868 MHz EU
  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true) { }
  }
  Serial.println("RadioLib init success");
}

void loop() {
  receiveMessages();
  delay(2000);
}

void ledOn() {
  digitalWrite(LED_INTERN, LOW);   // internal LED is reversed
  digitalWrite(LED_EXTERN, HIGH);
  Serial.println("LED ON");
}

void ledOff() {
  digitalWrite(LED_INTERN, HIGH);  // internal LED is reversed
  digitalWrite(LED_EXTERN, LOW);
  Serial.println("LED OFF");
}

void activateRelay() {
  ledOn();
  digitalWrite(RELAY, HIGH);
  Serial.println("Relay activated!");
}

void deactivateRelay() {
  ledOff();
  digitalWrite(RELAY, LOW);
  Serial.println("Relay deactivated!");
}

void receiveMessages() {
  Serial.println("Waiting for incoming messages...");
  uint8_t buf[256];
  size_t bufSize = sizeof(buf);

  int recvState = radio.receive(buf, bufSize);
  Serial.print("Received message with state: ");
  Serial.println(recvState);

  if (recvState == RADIOLIB_ERR_NONE) {
    size_t len = radio.getPacketLength();
    Serial.print("Packet length: ");
    Serial.println(len);

    if (len > 0 && len < bufSize) {
      buf[len] = '\0';
      String receivedMessage = String((char*)buf);
      receivedMessage.trim();
      Serial.println("Raw received message: " + receivedMessage);

      // Expect exactly "FAN:ON" or "FAN:OFF"
      if (receivedMessage.equalsIgnoreCase("FAN:ON")) {
        Serial.println("Command recognized: FAN:ON");
        activateRelay();
      } else if (receivedMessage.equalsIgnoreCase("FAN:OFF")) {
        Serial.println("Command recognized: FAN:OFF");
        deactivateRelay();
      } else {
        Serial.println("Unknown command format (expected FAN:ON or FAN:OFF)");
      }
    }
  } else if (recvState == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("Receive timeout, no message.");
  } else {
    Serial.print("Receive error, code: ");
    Serial.println(recvState);
  }
}
