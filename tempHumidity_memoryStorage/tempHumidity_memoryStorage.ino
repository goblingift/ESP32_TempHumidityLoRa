#include "Wire.h"
#include "DHT.h"
#include <string.h>
#define DHTTYPE DHT20
#define LED_EXTERN 1

DHT dht(DHTTYPE);

#define MAX_MEASUREMENTS 10

// Array to store humidity and temperature pairs
float measurements[MAX_MEASUREMENTS][2] = {0};  // [][0]: Humidity, [][1]: Temperature
int currentIndex = 0;

void setup() {
    Serial.begin(115200);
    pinMode(LED_EXTERN, OUTPUT);

    Serial.println("Starting XIAO ESP32S3 Temp and Humidity measurement!");

    Wire.begin();
    dht.begin();
}

void loop() {
    float temp_hum_val[2] = {0};

    ledOn();
    if (!dht.readTempAndHumidity(temp_hum_val)) {
        // Save current measurement
        measurements[currentIndex][0] = temp_hum_val[0];
        measurements[currentIndex][1] = temp_hum_val[1];
        printLastMeasurement(temp_hum_val[0], temp_hum_val[1]);

        currentIndex++;
        if (currentIndex >= MAX_MEASUREMENTS) {
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
}

void ledOn() {
    digitalWrite(LED_EXTERN, HIGH);
    Serial.println("LED ON");
}

void ledOff() {
    digitalWrite(LED_EXTERN, LOW);
    Serial.println("LED OFF");
}

void printLastMeasurement(float humidity, float temperature) {
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" *C");
}

void printStoredMeasurements() {
    Serial.println("Stored measurements:");
    for (int i = 0; i < MAX_MEASUREMENTS; i++) {
        Serial.print(i);
        Serial.print(": Humidity=");
        Serial.print(measurements[i][0]);
        Serial.print("%, Temp=");
        Serial.print(measurements[i][1]);
        Serial.println(" *C");
    }
    Serial.println("----------------------");
}
