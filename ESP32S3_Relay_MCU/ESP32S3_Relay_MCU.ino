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
  
  // NON-BLOCKING: 1 second timeout (64k * 15.625µs = ~1s)
  uint8_t buf[256];
  int recvState = radio.receive(buf, sizeof(buf), 64000);  // timeout param!
  
  Serial.print("RX state: ");
  Serial.println(recvState);
  
  if (recvState == RADIOLIB_ERR_NONE) {
    size_t len = radio.getPacketLength();
    Serial.print("Packet len: ");
    Serial.println(len);
    
    if (len > 0 && len < sizeof(buf)) {
      buf[len] = '\0';
      String msg = String((char*)buf);
      msg.trim();
      Serial.println("Raw: " + msg);
      
      // Filter FAN only
      if (!msg.startsWith("FAN:")) {
        Serial.println("Ignored: temp/humidity");
        return;
      }
      
      if (msg.equalsIgnoreCase("FAN:ON")) {
        activateRelay();
      } else if (msg.equalsIgnoreCase("FAN:OFF")) {
        deactivateRelay();
      }
    }
  } else if (recvState == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("Timeout (normal - no message)");
  }
}

