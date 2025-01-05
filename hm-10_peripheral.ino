#include <SoftwareSerial.h>

// Pin connections
const int rxPin = 11;  // RX pin of HM-10
const int txPin = 10;  // TX pin of HM-10

SoftwareSerial HM10(rxPin, txPin);

void setup() {
  // Initialize serial communication with the Arduino and HM-10
  Serial.begin(9600);
  HM10.begin(9600);

  // Wait for serial monitor to be ready
  delay(2000);

  // Configure the HM-10 module in peripheral mode and secure pairing
  HM10.println("AT+RESET");         // Reset the HM-10
  delay(1000);
  
  HM10.println("AT+ROLE1");         // Set to peripheral mode (slave mode)
  delay(500);
  
  HM10.println("AT+IMME1");         // Connectable only after manual advertisement
  delay(500);
  
  HM10.println("AT+ADVI4");         // Set advertising interval (1.28 seconds)
  delay(500);
  
  HM10.println("AT+TYPE2");         // Enable secure pairing (requires passkey)
  delay(500);
  
  HM10.println("AT+NAMEArduinoBT"); // Set the name to "ArduinoBT"
  delay(500);
  
  HM10.println("AT+RESET");         // Apply all the settings
  delay(1000);
  
  // Notify user
  Serial.println("Ready. Send 'ON' to start advertising.");
}

void loop() {
  // Check for serial input
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Read the serial command
    
    command.trim(); // Remove any extra spaces or newline characters
    
    if (command.equalsIgnoreCase("ON")) { // If the command is "ON"
      Serial.println("Starting advertisement...");
      
      // Start advertising to make the Arduino discoverable by the iPhone
      HM10.println("AT+ADVERT"); // Start advertising
      delay(5000);               // Advertise for 5 seconds
      
      // Stop advertising after a while
      HM10.println("AT+ADVOFF"); // Stop advertising
      Serial.println("Advertising stopped.");
    } else {
      Serial.println("Unknown command. Send 'ON' to start advertising.");
    }
  }
  
  // Any other continuous tasks or logic for your Arduino can go here
}
