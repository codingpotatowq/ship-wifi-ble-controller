# Bluetooth Low Energy (BLE) & WiFi Sender/Receiver

## Overview
This project enables wireless communication between an Arduino-based controller and a motorized system using **Bluetooth Low Energy (BLE)** and **WiFi (WiFiNINA)**. The system consists of two configurations:
1. **BLE Communication**: Used for short-range control.
2. **WiFi Communication**: Enables remote operation over a network.

## System Components
- **Sender (BLE & WiFi)**: Captures joystick inputs, switch states, and encoder values, then transmits them wirelessly.
- **Receiver (BLE & WiFi)**: Processes incoming data and controls motor movement accordingly.

## Features
- Real-time transmission of joystick and switch inputs.
- Automatic detection and connection to available devices.
- Control of motor speed and direction based on received data.
- JSON-based data transmission for flexibility and expandability.

---
## 1. BLE Communication

### **BLE Sender**
- Reads joystick X/Y values (mapped 0-255) and switch states.
- Reads rotary encoder for precise movement control.
- Sends data as a JSON string over BLE.
- Advertises a custom BLE service and characteristic.

### **BLE Receiver**
- Scans for the specific BLE device UUID.
- Establishes a connection and reads incoming data.
- Parses JSON data and extracts joystick values.
- Controls motors accordingly (forward, backward, left, right).

#### **BLE UUIDs**
- **Service UUID:** `19b10000-e8f2-537e-4f6c-d104768a1214`
- **Characteristic UUID:** `19b10000-e8f2-537e-4f6c-d104768a1214`

#### **Setup Instructions**
1. Upload the **BLE Sender** code to the Arduino Nano 33 IoT.
2. Upload the **BLE Receiver** code to the corresponding Arduino controlling the motors.
3. Power on both devices and ensure they are within BLE range (~10m).
4. Observe the serial monitor for connection and received data logs.

---
## 2. WiFi Communication

### **WiFi Sender**
- Uses the **WiFiNINA** library to send joystick and switch data via HTTP requests.
- Reads inputs from the joystick and switch.
- Formats data into a JSON object and sends it to a server (ESP32 or another Arduino acting as a receiver).

### **WiFi Receiver**
- Runs an HTTP server to listen for incoming requests.
- Parses JSON data and extracts joystick values.
- Controls motor movements accordingly.

#### **Setup Instructions**
1. Ensure the sender and receiver are connected to the same WiFi network.
2. Configure the receiver’s **IP address** in the sender’s code.
3. Upload the **WiFi Sender** code to the controller Arduino.
4. Upload the **WiFi Receiver** code to the motor control Arduino.
5. Power on both devices and observe the serial monitor for incoming requests.

---
## Example JSON Data
```json
{
  "joyX": 128,
  "joyY": 200,
  "switch": 1,
  "encoder": 15
}
```

## Libraries Required
- **ArduinoBLE** (For BLE communication)
- **WiFiNINA** (For WiFi communication)
- **ArduinoJSON** (For JSON parsing and handling)

## Potential Applications
- Remote-controlled robotics
- Smart home automation
- Industrial motor control

## Troubleshooting
### **BLE Issues**
- Ensure the sender and receiver are powered and within range.
- Check UUIDs to ensure they match.
- Use a BLE scanner app to verify advertisement.

### **WiFi Issues**
- Check WiFi credentials (SSID & Password) in the sender code.
- Ensure the receiver is running an HTTP server.
- Use `ping <receiver-ip>` to verify network connectivity.

## Future Improvements
- Implement encryption for secure communication.
- Add mobile app support for easier control.
- Introduce WebSocket-based real-time control.

---
This project demonstrates the **integration of BLE and WiFi** for robust wireless communication, making it ideal for various **IoT, robotics, and automation applications**.

