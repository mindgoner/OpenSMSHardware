#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define SIM800_TX_PIN 0 // D3 (D3 should be connected to TX (TX is right next to the GND pin at SIM800L module))
#define SIM800_RX_PIN 2 // D4 

SoftwareSerial sim800(SIM800_TX_PIN, SIM800_RX_PIN);
LiquidCrystal_I2C lcd(0x27,20, 4);
ESP8266WiFiMulti WiFiMulti;

/**
 *     CONFIGURABLE ZONE
 *
 */

bool SerialMonitorEnabled = true;
bool LCDFeatureEnabled = true;
String connectionType = "client/server"; // Available options: client/server (p2p [probably] will be available in the future)
String csaServerHostname = "192.168.0.1"; // Fill only if client/server (ip/domain names allowed)
String csaServerProtocol = "http"; // Fill only if client/server
String csaServerApiToken = "ABCDEFGHIJKLMNOPQRSTUWXYZ1234567"; // Fill only if client/server

// Connect to wifi that has internet access OR is in the same LAN where the OpenSMS Server is.
const char* WiFiSSID = "TestTest"; 
const char* WiFiPassword = "TestTest123";
const uint32_t WiFiTimeout = 20000; // Timeout time (it is recommended to be greather than 1000)

/**
 *     CODE ZONE
 *
 *     DO NOT CHANGE ANYTHING BELOW THAT LINE (unless you know what are you doing)
 */

bool commandFirstSuccess = false;
bool commandSecondSuccess = false;
bool commandThirdSuccess = false;
bool networkRegisterSuccess = false;
bool signalStrengthSuccess = false;

String requestBuilder = csaServerProtocol+"://"+csaServerHostname+"/?apitoken="+csaServerApiToken;

void setup() {
  delay(1000);

  if(LCDFeatureEnabled){
    Wire.begin(4, 5); //SDA = D, SCL
  }

  if(SerialMonitorEnabled){
    Serial.begin(115200);
  }

  sim800.begin(9600);

  if(LCDFeatureEnabled){
    // LCD Initials:
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Starting...");
    delay(500);
    lcd.clear();
  }

  if(SerialMonitorEnabled){
    Serial.println("Initializing SIM800...");
  }

  if(LCDFeatureEnabled){
    lcd.setCursor(0, 0);
    lcd.print("SIM800L Config:");
    lcd.setCursor(0, 1);
    lcd.print("AT test: ...");
  }
  while (!commandFirstSuccess) {
    commandFirstSuccess = sendCommand("AT", "OK", 2000);
  }
  
  if(SerialMonitorEnabled){
    Serial.print("AT test [OK]!\n");
  }
  if(LCDFeatureEnabled){
    lcd.setCursor(0, 1);
    lcd.print("AT test: [OK]");
    lcd.setCursor(0, 2);
    lcd.print("AT+CSCS test: ...");
  }

  while (!commandSecondSuccess) {
    commandSecondSuccess = sendCommand("AT+CSCS=\"GSM\"", "OK", 2000);
  }
  if(SerialMonitorEnabled){
    Serial.print("AT+CSCS test [OK]!\n");
  }

  if(LCDFeatureEnabled){
    lcd.setCursor(0, 2);
    lcd.print("AT+CSCS test: [OK]");
    lcd.setCursor(0, 3);
    lcd.print("AT+CMGF test: ...");
  }

  while (!commandThirdSuccess) {
    commandThirdSuccess = sendCommand("AT+CMGF=1", "OK", 2000);
  }
  if(SerialMonitorEnabled){
    Serial.print("AT+CMGF test [OK]!\n");
  }
  
  if(LCDFeatureEnabled){
    lcd.setCursor(0, 3);
    lcd.print("AT+CMGF test: [OK]");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SIM800L Config:");
    lcd.setCursor(0, 1);
    lcd.print("Registar test: ...");
  }

  while (!networkRegisterSuccess) {
    networkRegisterSuccess = checkNetworkRegistration();
  }
  if(SerialMonitorEnabled){
    Serial.print("Network Registered test [OK]!\n");
  }

  if(LCDFeatureEnabled){
    lcd.setCursor(0, 1);
    lcd.print("Registar test: [OK]");
    lcd.setCursor(0, 2);
    lcd.print("Signal test: ...");
  }

  while (!signalStrengthSuccess) {
    signalStrengthSuccess = checkSignalStrength();
  }
  if(SerialMonitorEnabled){
    Serial.print("Signal Strength test [OK]!\n");
  }


  if(LCDFeatureEnabled){
    lcd.setCursor(0, 2);
    lcd.print("Signal test: [OK]");
    lcd.setCursor(0, 3);
    lcd.print("SIM800L is working!");
    delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi...");
  }

  // Turning the wifi on and add MAC identifier to total request:
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WiFiSSID, WiFiPassword); 
  requestBuilder = "&deviceMAC="+requestBuilder+WiFi.macAddress();
  // WiFiMulti.addAP("AlterWiFi", "AlterWiFiPassword"); // You can add more hotspots to automatically switch to, if previous connection fails.

  //sendSMS("+1123123123", "System Booted");
}


/**
 *     MAIN LOOP
 *
 *     All the magic happens here
 */


void loop() {
  if (WiFiMulti.run(WiFiTimeout) == WL_CONNECTED) {
    
    if(SerialMonitorEnabled){
      Serial.print("WiFi connected: ");
      Serial.print(WiFi.SSID());
      Serial.print(" ");
      Serial.println(WiFi.RSSI());
    }

    if(LCDFeatureEnabled){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SSID:"+WiFi.SSID());
      lcd.setCursor(0, 1);
      lcd.print("IP:"+WiFi.localIP().toString());
      lcd.setCursor(0, 2);
      lcd.print("PH:"+WiFi.macAddress());
      lcd.setCursor(0, 3);
      lcd.print("Waiting for data...");
    }

    delay(2000);
  }else{

    if(SerialMonitorEnabled){
      Serial.println("WiFi not connected!");
    }
    if(LCDFeatureEnabled){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi not connected!:");
    }
    delay(2000);
  }
}


/**
 *     SIM800L FUNCTIONS ZONE
 *
 *     Functions used by SIM800L
 */


bool sendCommand(const char* cmd, const char* ack, unsigned int timeout) {

  if(SerialMonitorEnabled){
    Serial.print("Sending command: ");
    Serial.println(cmd);
  }
  
  sim800.println(cmd);
  if (ack && *ack) {
    unsigned long start = millis();
    while (millis() - start < timeout) {
      if (sim800.find(ack)) {
        
        if(SerialMonitorEnabled){
          Serial.print("Command executed successfully: ");
          Serial.println(cmd);
        }
        return true;
      }
    }
    
    if(SerialMonitorEnabled){
      Serial.print("Command timed out: ");
      Serial.println(cmd);
    }
  }
  return false;
}

bool checkNetworkRegistration() {
  
  if(SerialMonitorEnabled){
    Serial.println("Checking network registration...");
  }
  sim800.println("AT+CREG?");
  unsigned long start = millis();
  while (millis() - start < 10000) { // Wait up to 10 seconds
    if (sim800.available()) {
      String response = sim800.readString();
      
      if(SerialMonitorEnabled){
        Serial.println("Network registration response: " + response);
      }
      if (response.indexOf("+CREG: 0,1") > -1 || response.indexOf("+CREG: 0,5") > -1) {
        //Serial.println("Module is registered to the network.");
        return true;
      }
    }
  }
  
  if(SerialMonitorEnabled){
    Serial.println("Module failed to register to the network.");
  }
  return false;
}

bool checkSignalStrength() {
  
  if(SerialMonitorEnabled){
    Serial.println("Checking signal strength...");
  }
  sim800.println("AT+CSQ");
  unsigned long start = millis();
  while (millis() - start < 5000) {
    if (sim800.available()) {
      String response = sim800.readString();
      
      if(SerialMonitorEnabled){
        Serial.println("Signal strength response: " + response);
      }
      int index = response.indexOf("+CSQ: ");
      if (index != -1) {
        int rssi = response.substring(index + 6).toInt();
        if (rssi == 99) {
          if(SerialMonitorEnabled){
            Serial.println("Signal strength unknown or not detectable.");
          }
        } else {
          if(SerialMonitorEnabled){
            Serial.print("Signal strength (RSSI): ");
            Serial.println(rssi);
          }
          if(rssi > 10){
            return true;
          }
        }
        return false;
      }
    }
  }
  
  if(SerialMonitorEnabled){
    Serial.println("Failed to get signal strength.");
  }
  return false;
}

void sendSMS(const char* number, const char* text) {
  
  if(SerialMonitorEnabled){
    Serial.print("Sending SMS to: ");
    Serial.println(number);
  }
  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(100);
  sim800.print(text);
  delay(100);
  sim800.write(26);
  delay(5000);
  
  if(SerialMonitorEnabled){
    Serial.println("SMS sent");
  }
}
