#include "secrets.h"
#include "WiFi.h"
#include "BlynkSimpleEsp32.h"
#include <RadioLib.h>

#define LED_BUILTIN 21

// LoRa SX1262 pins for Wio SX-1262 + XIAO ESP32S3 B2B
SX1262 radio = new Module(41, 39, 42, 40);

BlynkTimer timer;
unsigned long lastSensorTime = 0;
unsigned long pendingFanStateTime = 0;
int pendingFanState = -1;  // -1 = no pending
const unsigned long FAN_DELAY_MS = 10000;

float receivedTemp = 0.0;
float receivedHumidity = 0.0;
bool newData = false;

BLYNK_WRITE(V3) {
  int newFanState = param.asInt();

  if (newFanState != pendingFanState) {
    pendingFanState = newFanState;
    pendingFanStateTime = millis();
    Serial.print("FAN command QUEUED: ");
    Serial.println(newFanState ? "ON" : "OFF");
  }

  checkAndSendFanCommand();
}

void checkAndSendFanCommand() {
  unsigned long now = millis();

  // Ready to send?
  if (pendingFanState != -1 && (now - lastSensorTime >= FAN_DELAY_MS)) {
    Serial.print("SENDING PENDING FAN: ");
    Serial.println(pendingFanState ? "ON" : "OFF");

    String loraMsg = (pendingFanState == 1) ? "FAN:ON" : "FAN:OFF";

    radio.standby();
    int txState = radio.transmit(loraMsg);

    if (txState == RADIOLIB_ERR_NONE) {
      Serial.println("LoRa TX SUCCESS");
    } else {
      Serial.print("TX FAILED | Code: ");
      Serial.println(txState);
    }

    Blynk.virtualWrite(V3, pendingFanState);
    pendingFanState = -1;  // Clear queue
  } else if (pendingFanState != -1) {
    unsigned long remaining = (lastSensorTime + FAN_DELAY_MS - now) / 1000;
    Serial.print("Waiting ");
    Serial.print(remaining);
    Serial.println("s for FAN command...");
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Init WiFi + Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Blynk connected!");

  // Sync V3 state from cloud on startup
  Blynk.syncVirtual(V3);

  // Init LoRa receiver
  int status = radio.begin(868.0);  // 868 MHz EU
  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("LoRa init failed!");
    while (true) {}
  }
  Serial.println("LoRa receiver ready!");
}

void loop() {
  Blynk.run();
  receiveLoRaData();

  if (newData) {
    Blynk.virtualWrite(V1, receivedTemp);
    Blynk.virtualWrite(V2, receivedHumidity);
    lastSensorTime = millis();  // ← Update here!
    Serial.printf("Sent to Blynk: T%.2f H%.2f\n", receivedTemp, receivedHumidity);
    newData = false;
  }

  checkAndSendFanCommand();
  delay(100);
}

void receiveLoRaData() {
  uint8_t buf[64];
  int state = radio.receive(buf, 64);

  if (state == RADIOLIB_ERR_NONE) {
    size_t len = radio.getPacketLength();
    if (len > 0 && len < 64) {
      buf[len] = '\0';
      String msg = String((char*)buf);
      msg.trim();

      Serial.print("LoRa received: ");
      Serial.println(msg);

      parseTempHumidity(msg);

      // Mark sensor data time - FAN commands delayed 10s
      if (newData) {
        lastSensorTime = millis();
      }
    }
  }
}

void parseTempHumidity(String msg) {
  int tPos = msg.indexOf('T');
  int hPos = msg.indexOf('H');

  if (tPos == 0 && hPos > tPos) {
    String tempStr = msg.substring(tPos + 1, hPos);
    String humStr = msg.substring(hPos + 1);

    receivedTemp = tempStr.toFloat();
    receivedHumidity = humStr.toFloat();

    if (!isnan(receivedTemp) && !isnan(receivedHumidity)) {
      newData = true;
      Serial.printf("Parsed: Temp=%.2f°C, Humidity=%.2f%%\n", receivedTemp, receivedHumidity);
    }
  } else {
    Serial.println("Invalid format (expected Txx.xxHyy.yy)");
  }
}
