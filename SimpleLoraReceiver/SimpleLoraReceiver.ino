#include <RadioLib.h>

#define LED_INTERN 21

SX1262 radio = new Module(41, 39, 42, 40);  //support for the Wio_SX-1262 & Xiao ESP32S3 with B2B connector.

void setup() {
  Serial.begin(115200);
  Serial.println("Starting SX1262 LoRa Receiver with RadioLib");

  int status = radio.begin(868.0);  // Frequency 868 MHz for EU

  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true)
      ;
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
  digitalWrite(LED_INTERN, LOW);  // internal LED is reversed
  Serial.println("LED ON");
}

void ledOff() {
  digitalWrite(LED_INTERN, HIGH);  // internal LED is reversed
  Serial.println("LED OFF");
}

// Returns true if idStr is exactly 10 characters long and all digits
bool isValidUniqueId(const String& idStr) {
  if (idStr.length() != 10) {
    return false;
  }
  for (int i = 0; i < 10; i++) {
    if (!isDigit(idStr.charAt(i))) {
      return false;
    }
  }
  return true;
}

void receiveMessages() {
  Serial.println("Waiting for incoming messages...");
  uint8_t buf[256];  // Buffer for incoming message
  size_t bufSize = sizeof(buf);
  int recvState = radio.receive(buf, bufSize);
  Serial.print("Received message with state: ");
  Serial.println(recvState);


  if (recvState == RADIOLIB_ERR_NONE) {
    Serial.println("xxx: No errors");
    size_t len = radio.getPacketLength();  // Get length of received packet
    Serial.print("xxx: length:");
    Serial.println(len);

    if (len > 0 && len < bufSize) {
      buf[len] = '\0';  // Null-terminate the received data
      String receivedMessage = String((char*)buf);
      receivedMessage.trim();
      Serial.println("Received message: " + receivedMessage);

      int sepIndex = receivedMessage.indexOf(':');
      if (sepIndex == -1) {
        Serial.println("Invalid format: Missing ID separator ':'");
        return;
      }

      String uniqueId = receivedMessage.substring(0, sepIndex);
      if (!isValidUniqueId(uniqueId)) {
        Serial.println("Invalid unique ID");
        return;
      }
      Serial.print("Unique ID: ");
      Serial.println(uniqueId);

      String tempHumStr = receivedMessage.substring(sepIndex + 1);
      if (validateTempHumString(tempHumStr)) {
        Serial.println("Valid temperature/humidity string");
      } else {
        Serial.println("Invalid temperature/humidity format");
      }
    }
  } else if (recvState == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println("Receive timeout, no message.");
  } else {
    Serial.print("Receive error, code: ");
    Serial.println(recvState);
  }
}



bool validateTempHumString(const String& input) {
  // Must start with 'T'
  if (input.length() < 8) return false;  // Minimum length for "T0.0H0.0"
  if (input.charAt(0) != 'T') return false;

  // Find position of 'H'
  int hPos = input.indexOf('H');
  if (hPos == -1) return false;

  // Extract temperature and humidity substrings
  String tempStr = input.substring(1, hPos);
  String humStr = input.substring(hPos + 1);

  // Convert to float safely and verify that the whole substring was numeric
  float tempVal = tempStr.toFloat();
  float humVal = humStr.toFloat();

  // Check that strs represent float numbers with correct decimal formatting (simple check)
  if (tempStr.length() < 3 || humStr.length() < 3) return false;

  // Check that after conversion, the value is not zero because toFloat returns 0 for invalid strings
  if (!(tempVal != 0 || tempStr == "0" || tempStr == "0.0")) return false;
  if (!(humVal != 0 || humStr == "0" || humStr == "0.0")) return false;

  // Extra: check no extra invalid chars (only digits, '.' and possibly '-' if negative)
  for (int i = 0; i < tempStr.length(); i++) {
    char c = tempStr.charAt(i);
    if (!isDigit(c) && c != '.') return false;
  }
  for (int i = 0; i < humStr.length(); i++) {
    char c = humStr.charAt(i);
    if (!isDigit(c) && c != '.') return false;
  }

  return true;
}


void printSignalStrength() {
  // print RSSI (Received Signal Strength Indicator)
  Serial.print(F("[SX1262] RSSI:\t\t"));
  Serial.print(radio.getRSSI());
  Serial.println(F(" dBm"));

  // print SNR (Signal-to-Noise Ratio)
  Serial.print(F("[SX1262] SNR:\t\t"));
  Serial.print(radio.getSNR());
  Serial.println(F(" dB"));
}