#define LED_BUILTIN 21  // Onboard LED is connected to GPIO21 on XIAO ESP32S3
#define LED_EXTERN 1
#define RELAY 2

void setup() {
  Serial.begin(115200);          // Start serial communication at 115200 baud rate
  pinMode(LED_BUILTIN, OUTPUT);  // Set the LED pin as an output
  pinMode(LED_EXTERN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  Serial.println("Hello from XIAO ESP32S3!");  // Print message once on start
}

void loop() {

  activateRelay();
  delay(2000);

  deactivateRelay();
  delay(500);

  activateRelay();
  delay(500);

  deactivateRelay();
  delay(500);
}

void activateRelay() {
  digitalWrite(LED_BUILTIN, LOW);  // builtin LED is powered-on with LOW
  digitalWrite(LED_EXTERN, HIGH);
  digitalWrite(RELAY, HIGH);
  Serial.println("Relay activated!");
}

void deactivateRelay() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_EXTERN, LOW);
  digitalWrite(RELAY, LOW);
  Serial.println("Relay deactivated!");
}