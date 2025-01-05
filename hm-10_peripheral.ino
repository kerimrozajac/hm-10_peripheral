#include <SoftwareSerial.h>

// Constants
const int RX_PIN = D4;   // RX pin of HM-10
const int TX_PIN = D5;   // TX pin of HM-10
const String EXPECTED_ROLE = "role3";  // Expected role
const String EXPECTED_PIN = "1234";   // Expected PIN

// HM-10 Serial Interface
SoftwareSerial hm10(RX_PIN, TX_PIN);

// LED control
const int LED_PIN = LED_BUILTIN; // Onboard LED pin

// Communication states
enum DataState { INVALID, LED_ON, LED_OFF };

DataState currentState = INVALID;
DataState previousState = INVALID;  // Track the previous state

// Function Prototypes
void initializeHM10();
void handleHM10Response();
void controlLED();
DataState parseInputData(const String& data);
void checkBluetoothConnection();
void waitForStartCommand();

// Setup function
void setup() {
  Serial.begin(9600);  // Serial Monitor
  Serial.println("HM10 serial started at 9600");
  
  hm10.begin(9600);  // Set HM10 serial baud rate to 9600
  pinMode(LED_PIN, OUTPUT);  // Set onboard LED as output
  digitalWrite(LED_PIN, HIGH);  // Initially turn off LED

  // Wait for STARTAT command before initializing HM-10
  //waitForStartCommand();
  
  // Initialize HM-10 module after receiving STARTAT
  //initializeHM10();
}


// Main loop function
void loop() {

  // Read from Serial Monitor if available
  if (Serial.available()) {
    String inData = Serial.readStringUntil('\n');  // Read input until newline
    handleSerialInput(inData);
  }

  // Handle LED state based on current state
  controlLED();

}

// Function to enter AT command mode
void enterATCommandMode() {
  Serial.println("Entering AT Command Mode. Type AT commands to interact with HM-10.");

  // Wait for AT commands from the Serial Monitor and forward them to the HM-10
  while (true) {
    if (Serial.available()) {
      String atCommand = Serial.readStringUntil('\n');
      atCommand.trim();  // Remove extra spaces/newlines
      if (atCommand == "ENDAT") {
        Serial.println("Exiting AT Command Mode.");
        return;
        }

      if (atCommand.length() > 0) {
        hm10.println(atCommand);  // Send AT command to HM-10
        Serial.println("Sent to HM-10: " + atCommand);

        // Wait for the response from HM-10
        waitForResponse();
        delay(100);  // Add a small delay to avoid overwhelming the HM-10
      }
      
    }
  }
}

// Function to wait for and display HM-10 responses
void waitForResponse() {
  String response = "";
  
  // Wait for data from HM-10
  unsigned long startTime = millis();
  while (millis() - startTime < 2000) {  // Wait up to 2 seconds for response
    if (hm10.available()) {
      response += (char)hm10.read();
    }
  }

  if (response.length() > 0) {
    Serial.println("HM-10: " + response);  // Display response
  } else {
    Serial.println("No response from HM-10 within timeout period.");
  }
}


// Function to wait for "STARTAT" command from Serial Monitor
void waitForStartCommand() {
  String command = "";
  while (command != "STARTAT") {
    if (Serial.available()) {
      command = Serial.readStringUntil('\n');
      command.trim();  // Clean input by removing leading/trailing whitespace
      if (command == "STARTAT") {
        Serial.println("AT Command Mode Activated!");
      } else {
        Serial.println("Waiting for STARTAT command...");
      }
    }
  }
}



// Function to initialize the HM-10 module
void initializeHM10() {
  // Send AT commands to initialize the HM-10
  sendATCommand("AT+RESET", 1000);   // Reset HM-10
  sendATCommand("AT+ROLE3", 1000);   // Set role to role3
  
  // Optional: Verify the role is correctly set
  sendATCommand("AT+ROLE?", 500);  // Query the role
}

// Function to send AT commands to HM-10 and wait for a response
void sendATCommand(const String& command, unsigned long delayTime) {
  Serial.println("Sending command: " + command);
  hm10.println(command);  // Send the AT command
  delay(delayTime);       // Wait for the module to process the command

  // Optional: Read response from HM-10 to verify success
  while (hm10.available()) {
    String response = hm10.readStringUntil('\n');
    Serial.println("Response: " + response);
  }
}

// Function to handle incoming responses from HM-10
void handleHM10Response() {
  if (hm10.available() > 0) {
    char appData = hm10.read();
    String inData = String(appData);  // Convert to string for easy processing
    Serial.write(appData);  // Output data to serial monitor

    // Parse and process the incoming data
    currentState = parseInputData(inData);
  }
}

// Function to handle serial input from Serial Monitor
void handleSerialInput(const String& data) {
  String trimmedData = data;
  trimmedData.trim();  // Remove leading and trailing whitespace or newline characters

  // If serial input is invalid, print an error message
  if (!isValidInput(trimmedData)) {
    Serial.println("Invalid input received: " + trimmedData);
    return;
  }

  if (trimmedData == "STARTAT") {
      enterATCommandMode();
    }

  currentState = parseInputData(trimmedData);
}

// Function to check if the input is valid (only "N" or "F")
bool isValidInput(const String& data) {
  // Only accept 'N' or 'F' as valid inputs for LED control
  return data == "N" || data == "F" || data == "STARTAT";
}

// Function to parse the input data for LED control
DataState parseInputData(const String& data) {
  if (data == "N") {
    return LED_ON;
  } else if (data == "F") {
    return LED_OFF;
  } else {
    return INVALID;  // Invalid input data
  }
}

// Function to control the LED based on the current state
void controlLED() {
  // Only act if the state has changed
  if (currentState != previousState) {
    switch (currentState) {
      case LED_ON:
        Serial.println("LED ON");
        digitalWrite(LED_PIN, LOW);  // Turn on the LED
        break;

      case LED_OFF:
        Serial.println("LED OFF");
        digitalWrite(LED_PIN, HIGH);  // Turn off the LED
        break;

      case INVALID:
        // No action needed for invalid state
        break;
    }

    // Update the previous state
    previousState = currentState;
  }
}

