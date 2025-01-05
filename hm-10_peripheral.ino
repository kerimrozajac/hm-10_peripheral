#include <SoftwareSerial.h>

// Define HM-10 pins
const int RX_PIN = D4;   // RX pin of HM-10
const int TX_PIN = D5;   // TX pin of HM-10
SoftwareSerial hm10(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);  // For Serial Monitor
  hm10.begin(9600);    // For HM-10 module (default baud rate)

  Serial.println("HM-10 Test: Send 'AT' to check response");
}

void loop() {
  // Check for data from HM-10
  if (hm10.available()) {
    String dataFromHM10 = hm10.readString();
    Serial.println("HM-10: " + dataFromHM10);
  }

  // Check for data from Serial Monitor
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    hm10.println(command);  // Send command to HM-10
    Serial.println("Sent to HM-10: " + command);
  }
}
