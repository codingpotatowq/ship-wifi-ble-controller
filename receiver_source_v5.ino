#include <ArduinoBLE.h>

// Motor 1 Control
#define M1_PWM 2
#define M1_IN1 3
#define M1_IN2 4

// Motor 2 Control
#define M2_PWM 5
#define M2_IN1 6
#define M2_IN2 7

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Setup for BLE
  BLE.begin();
  Serial.println("Bluetooth® Low Energy Central - Motor control");

  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  Serial.println("BLE connecting...");

  if (!BLE.begin()) {
    Serial.println("Failed to start BLE! Check power and connections.");
    while (1);
  }

  // Start scanning for peripherals
  Serial.println("Scanning for BLE devices...");
  BLE.scan();

  // MOTOR DEFINITION (EDIT)
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);

  // Stop Motors Initially
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, LOW);
}

// Checking for peripherals
void loop() {
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    // Print all available services
    Serial.print("Found Device: ");
    Serial.print(peripheral.address());
    Serial.print(" | Name: ");
    Serial.print(peripheral.localName());
    
    // Print available service UUIDs
    if (peripheral.hasAdvertisedServiceUuid()) {
      Serial.print(" | UUIDs: ");
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        Serial.print(peripheral.advertisedServiceUuid(i));
        Serial.print(" ");
      }
    } else {
      Serial.print(" | No UUIDs advertised");
    }
    Serial.println();
    delay(2000);  // Delay to prevent spamming

    
    BLE.stopScan();
    controlled(peripheral);
    
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
  }
}

// BLE connection
void controlled(BLEDevice peripheral) {
  Serial.println("Checking connection state...");
  if (peripheral.connected()) {
    Serial.println("Already connected! Disconnecting first...");
    peripheral.disconnect();
    delay(3000);
  }
  
  Serial.println("Connecting ...");

  // Checking RSSI 
  Serial.print("Signal Strength (RSSI): ");
    Serial.println(peripheral.rssi());
    if (peripheral.connect()) {
    Serial.println("Connected! yay! :D");
  }

  // Peripheral Attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Discovered Attributes:");
    for (int i = 0; i < peripheral.serviceCount(); i++) {
      Serial.print("Service UUID: ");
      Serial.println(peripheral.service(i).uuid());

      for (int j = 0; j < peripheral.service(i).characteristicCount(); j++) {
        Serial.print("Chracteristic UUID: ");
        Serial.println(peripheral.service(i).characteristic(j).uuid());
      }
    }
  }
  if (!peripheral.discoverAttributes()) {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  BLECharacteristic sensorDataCharacteristic = peripheral.characteristic("19b10000-e8f2-537e-4f6c-d104768a1214");

  // MOTOR CONTROLS FOR SERIAL MONITOR (EDIT IF CHANGE FORMATTING)
  while (peripheral.connected()) {
    if (sensorDataCharacteristic.read()) {
      String receivedData = String((char*)sensorDataCharacteristic.value());
      Serial.println("Received: " + receivedData);

      // PRINT JSON to Integer values
      int joyX = getValue(receivedData, "joyX");
      int joyY = getValue(receivedData, "joyY");
      int switchState = getValue(receivedData, "switch");

      controlMotors(joyX, joyY);

      Serial.print("X: "); Serial.print(joyX);
      Serial.print(" | Y: "); Serial.print(joyY);
      Serial.print(" | Switch: "); Serial.println(switchState);
    }
  }
  
  Serial.println("Peripheral disconnected.");
}

// CONVERSION FROM JSON TO INTEGER VALUES
int getValue(String data, String key) {
  int start = data.indexOf("\"" + key + "\":");
  if (start == -1) return 0;
  start += key.length() + 3;
  int end = data.indexOf(",", start);
  if (end == -1) end = data.indexOf("}", start);
  return data.substring(start, end).toInt();
}

// INITIAL MOTOR SPEEDS
void controlMotors(int xAxis, int yAxis) {
  int motorSpeedA = 0;
  int motorSpeedB = 0;

  // MOTOR COMMANDS (EDIT)
  if (xAxis > 135) {     // Forward
    moveForward(255, 255);
  } else if (xAxis < 120) { // Backward
    moveBackward(255, 255);
  }

  if (yAxis > 135) {   // Right turn
    motorSpeedA += 50;
    motorSpeedB -= 50;
  } else if (yAxis < 120) { // Left turn
    motorSpeedA -= 50;
    motorSpeedB += 50;
  }

  motorSpeedA = constrain(motorSpeedA, 0, 255);
  motorSpeedB = constrain(motorSpeedB, 0, 255);

  analogWrite(M1_PWM, motorSpeedA);
  analogWrite(M2_PWM, motorSpeedB);
}
// DIGITALWRITE = SET MOTOR DIRECTION, ANALOGWRITE = SET MOTOR SPEED
void stopMotors() {
  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, LOW);
  analogWrite(M1_PWM, 0);
  analogWrite(M2_PWM, 0);
}

void moveForward(int speedA, int speedB) {
  stopMotors();
  delay(10);

  digitalWrite(M1_IN1, HIGH);
  digitalWrite(M1_IN2, LOW);
  digitalWrite(M2_IN1, HIGH);
  digitalWrite(M2_IN2, LOW);
  analogWrite(M1_PWM, speedA);
  analogWrite(M2_PWM, speedB);
}

void moveBackward(int speedA, int speedB) {
  stopMotors();
  delay(10);

  digitalWrite(M1_IN1, LOW);
  digitalWrite(M1_IN2, HIGH);
  digitalWrite(M2_IN1, LOW);
  digitalWrite(M2_IN2, HIGH);
  analogWrite(M1_PWM, speedA);
  analogWrite(M2_PWM, speedB);
}

