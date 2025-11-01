#include "Wire.h"
#include "DHT.h"
#define DHTTYPE DHT20
#define LED_EXTERN 1

DHT dht(DHTTYPE);

void setup() {
    Serial.begin(115200);
    pinMode(LED_EXTERN, OUTPUT);

    Serial.println("Starting XIAO ESP32S3 Temp and Humidity measurement!");

    Wire.begin();
    dht.begin();
}

void loop() {
    float temp_hum_val[2] = {0};
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

    ledOn1sec();
    if (!dht.readTempAndHumidity(temp_hum_val)) {
        Serial.print("Humidity: ");
        Serial.print(temp_hum_val[0]);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(temp_hum_val[1]);
        Serial.println(" *C");
    } else {
        Serial.println("Failed to get temprature and humidity value.");
    }
    ledOff();

    delay(1500);
}

void ledOn1sec() {
    digitalWrite(LED_EXTERN, HIGH);   // Turn LED on (active low)
    Serial.println("LED ON");
}

void ledOff() {
    digitalWrite(LED_EXTERN, LOW);   // Turn LED on (active low)
    Serial.println("LED OFF");
}