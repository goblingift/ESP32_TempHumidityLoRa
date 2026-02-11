#include "secrets.h"
#include "WiFi.h"
#include "BlynkSimpleEsp32.h"
#include <RadioLib.h>

#define LED_BUILTIN 21

// LoRa SX1262 pins for Wio SX-1262 + XIAO ESP32S3 B2B
SX1262 radio = new Module(41, 39, 42, 40);

BlynkTimer timer;

float receivedTemp = 0.0;
float receivedHumidity = 0.0;
bool newData = false;

BLYNK_WRITE(V3) {
  int fanState = param.asInt();
  
  Serial.print("=== FAN COMMAND RECEIVED ===");
  Serial.print(" | Blynk V3: ");
  Serial.print(fanState);
  Serial.print(" | LoRa msg: ");
  
  String loraMsg = (fanState == 1) ? "FAN:ON" : "FAN:OFF";
  Serial.println(loraMsg);
  
  Serial.println("Sending LoRa...");
  
  // BLOCKING TRANSMIT - guaranteed delivery!
  radio.standby();  // Exit RX
  int txState = radio.transmit(loraMsg);  // Send + wait complete!
  
  if (txState == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa TX SUCCESS");
    Serial.print("Datarate: ");
    Serial.print(radio.getDataRate());
    Serial.println(" bps");
  } else {
    Serial.print("LoRa TX FAILED | Code: ");
    Serial.println(txState);
  }
  
  Serial.println("========================");
  Blynk.virtualWrite(V3, fanState);
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

  // Non-blocking receive check every loop
  receiveLoRaData();

  // Send to Blynk if new data arrived
  if (newData) {
    Blynk.virtualWrite(V1, receivedTemp);      // temperature
    Blynk.virtualWrite(V2, receivedHumidity);  // relative humidity
    Serial.printf("Sent to Blynk: T%.2f H%.2f\n", receivedTemp, receivedHumidity);
    newData = false;
  }

  delay(100);  // Small delay to prevent overwhelming
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

      // Parse "Txx.xxHyy.yy"
      parseTempHumidity(msg);
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
