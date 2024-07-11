# OpenSMSHardware

OpenSMSHardware provides firmware for various microcontrollers to process API requests from OpenSMS or OpenSMSServer and send SMS messages.

## Available Devices

### 1. ESP8266 with SIM800L Module

![ESP8266-SIM800L Wiring Diagram](https://github.com/mindgoner/OpenSMSHardware/blob/master/ESP8266-SIM800L/scheme.png)

#### Description

This device utilizes an ESP8266 microcontroller along with a SIM800L GSM module to send SMS messages. The ESP8266 handles Wi-Fi connectivity and communicates with the SIM800L module to transmit SMS messages over the GSM network.

#### Features

- Integrated Wi-Fi connectivity for easy network access.
- SIM800L module for GSM communication.
- Example firmware provided for sending SMS messages via OpenSMSServer or directly through OpenSMS API.

#### Getting Started

1. **Hardware Setup:**
   - Connect the ESP8266 and SIM800L module as shown in the wiring diagram above.
   
2. **Software Setup:**
   - Clone this repository and open the firmware code in your preferred IDE (e.g., Arduino IDE).
   - Configure your Wi-Fi credentials and API endpoint settings in the firmware code.
   
3. **Upload Firmware:**
   - Upload the compiled firmware to your ESP8266 microcontroller.
   
4. **Usage:**
   - The device will connect to the specified Wi-Fi network and communicate with OpenSMSServer or OpenSMS API to send SMS messages.

![ESP8266-SIM800L Breadboard Setup](https://github.com/mindgoner/OpenSMSHardware/blob/master/ESP8266-SIM800L/breadboard.png)

## Related Projects

- [OpenSMS](https://github.com/yourusername/OpenSMS): A PHP library for sending SMS messages from Symfony or Laravel projects.
- [OpenSMSServer](https://github.com/yourusername/OpenSMSServer): A Laravel application that acts as a gateway for sending SMS messages from IoT modules located in different networks.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
