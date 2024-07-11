#include <SoftwareSerial.h>

#define SIM800_TX_PIN 0 // D4 -> GPIO0
#define SIM800_RX_PIN 2 // D3 -> GPIO4

SoftwareSerial sim800(SIM800_TX_PIN, SIM800_RX_PIN);

void setup() {
  delay(1000);

  Serial.begin(115200);
  sim800.begin(9600);
  
  Serial.println("Initializing SIM800...");

  sendCommand("AT", "OK", 2000);
  sendCommand("AT+CSCS=\"GSM\"", "OK", 2000);
  sendCommand("AT+CMGF=1", "OK", 2000);

  checkNetworkRegistration();
  checkSignalStrength();

  sendSMS("TYPE_NUMBER_HERE", "ok");
}

void loop() {
  // Nothing to do here
}

void sendCommand(const char* cmd, const char* ack, unsigned int timeout) {
  Serial.print("Sending command: ");
  Serial.println(cmd);
  
  sim800.println(cmd);
  if (ack && *ack) {
    unsigned long start = millis();
    while (millis() - start < timeout) {
      if (sim800.find(ack)) {
        Serial.print("Command executed successfully: ");
        Serial.println(cmd);
        return;
      }
    }
    Serial.print("Command timed out: ");
    Serial.println(cmd);
  }
}

void checkNetworkRegistration() {
  Serial.println("Checking network registration...");
  sim800.println("AT+CREG?");
  unsigned long start = millis();
  while (millis() - start < 10000) { // Wait up to 10 seconds
    if (sim800.available()) {
      String response = sim800.readString();
      Serial.println("Network registration response: " + response);
      if (response.indexOf("+CREG: 0,1") > -1 || response.indexOf("+CREG: 0,5") > -1) {
        Serial.println("Module is registered to the network.");
        return;
      }
    }
  }
  Serial.println("Module failed to register to the network.");
}

void checkSignalStrength() {
  Serial.println("Checking signal strength...");
  sim800.println("AT+CSQ");
  unsigned long start = millis();
  while (millis() - start < 5000) {
    if (sim800.available()) {
      String response = sim800.readString();
      Serial.println("Signal strength response: " + response);
      int index = response.indexOf("+CSQ: ");
      if (index != -1) {
        int rssi = response.substring(index + 6).toInt();
        if (rssi == 99) {
          Serial.println("Signal strength unknown or not detectable.");
        } else {
          Serial.print("Signal strength (RSSI): ");
          Serial.println(rssi);
        }
        return;
      }
    }
  }
  Serial.println("Failed to get signal strength.");
}

void sendSMS(const char* number, const char* text) {
  Serial.print("Sending SMS to: ");
  Serial.println(number);
  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(100);
  sim800.print(text);
  delay(100);
  sim800.write(26);
  delay(5000);
  Serial.println("SMS sent");
}
