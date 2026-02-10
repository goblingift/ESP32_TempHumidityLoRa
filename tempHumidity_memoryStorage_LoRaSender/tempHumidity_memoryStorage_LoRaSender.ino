#include "Wire.h"
#include "DHT.h"
#include <RadioLib.h>

#define DHTTYPE DHT20
#define LED_BUILTIN 21

SX1262 radio = new Module(41, 39, 42, 40);  // Wio SX-1262 & XIAO ESP32S3 B2B

DHT dht(DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting XIAO ESP32S3 Temp and Humidity measurement!");
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Starting SX1262 LoRa Sender!");
  int status = radio.begin(868.0);  // 868 MHz EU
  if (status != RADIOLIB_ERR_NONE) {
    Serial.println("RadioLib init failed");
    while (true) { }
  }
  Serial.println("RadioLib init success");

  Serial.println("Starting DHT20 sensor!");
  Wire.begin();
  dht.begin();
}

void loop() {
  float humidity = 0.0;
  float temperature = 0.0;

  ledOn();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  
  if (!isnan(humidity) && !isnan(temperature)) {
    printTempHumidityMeasurement(humidity, temperature);
    sendDataViaLoRa(temperature, humidity);
  } else {
    Serial.println("Failed to get temperature and humidity value.");
  }
  ledOff();

  delay(60000);  // Wait 60 seconds before next reading
}

void sendDataViaLoRa(float temperature, float humidity) {
  char buf[20];
  // Format as "Txx.xxHyy.yy"
  sprintf(buf, "T%.2fH%.2f", temperature, humidity);

  int state = radio.startTransmit(buf);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Message sent successfully! Payload: ");
    Serial.println(buf);
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
