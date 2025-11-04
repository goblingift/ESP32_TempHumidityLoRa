#include "Wire.h"
#include "DHT.h"
#include <string.h>
#include <RadioLib.h>

#define DHTTYPE DHT20
#define LED_BUILTIN 21

SX1262 radio = new Module(41, 39, 42, 40);  //support for the Wio_SX-1262 & Xiao ESP32S3 with B2B connector.

DHT dht(DHTTYPE);

#define MAX_MEASUREMENTS 10

// Array to store humidity and temperature pairs
float measurements[MAX_MEASUREMENTS][2] = { 0 };  // [][0]: Humidity, [][1]: Temperature
int currentIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting XIAO ESP32S3 Temp and Humidity measurement!");
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Starting SX1262 LoRa Sender!");
  int status = radio.begin(868.0);  // Frequency 868 MHz for EU

  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true);
  }
  Serial.println("RadioLib init success");

  Serial.println("Starting DHT20 sensor!");
  Wire.begin();
  dht.begin();
}

void loop() {
  float temp_hum_val[2] = { 0 };

  ledOn();
  if (!dht.readTempAndHumidity(temp_hum_val)) {
    // Save current measurement
    measurements[currentIndex][0] = temp_hum_val[0];
    measurements[currentIndex][1] = temp_hum_val[1];
    printTempHumidityMeasurement(temp_hum_val[0], temp_hum_val[1]);

    currentIndex++;
    if (currentIndex >= MAX_MEASUREMENTS) {
      Serial.println("Maximum number of measurements done:");
      printStoredMeasurements();
      currentIndex = 0;
      memset(measurements, 0, sizeof(measurements));
      Serial.println("Erased all measurements!");
    }
  } else {
    Serial.println("Failed to get temperature and humidity value.");
  }
  ledOff();

  delay(1000);
  sendDataViaLoRa(String(temp_hum_val[0], 2), String(temp_hum_val[1], 2));


  delay(2000);
}

void sendDataViaLoRa(String humidity, String temperature) {
  String transmitString = "T" + temperature + "H" + humidity;
  int state = radio.startTransmit(transmitString);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.print("Failed to send, code: ");
    Serial.println(state);
  }
}

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED ON");
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED OFF");
}

void printTempHumidityMeasurement(float humidity, float temperature) {
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
}

void printStoredMeasurements() {
  for (int i = 0; i < MAX_MEASUREMENTS; i++) {
    printTempHumidityMeasurement(measurements[i][0], measurements[i][1]);
  }
  Serial.println("----------------------");
}
