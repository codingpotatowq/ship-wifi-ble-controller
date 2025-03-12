#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

// WiFi Credentials (Replace with your own)
char ssid[] = "TP-Link_6373";      // Your WiFi network name
char pass[] = "haha_not_gonna_leak_it";  // Your WiFi password
int status = WL_IDLE_STATUS;

// Server on port 78
WiFiServer server(80);

// Motor 1 Control
#define M1_PWM 2
#define M1_IN1 3
#define M1_IN2 4

// Motor 2 Control
#define M2_PWM 5
#define M2_IN1 6
#define M2_IN2 7

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

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
  Serial.println("Server started, waiting for connections...");

  // MOTOR SETUP
  pinMode(M1_PWM, OUTPUT);
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_PWM, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);

  // Stop Motors Initially
  stopMotors();
}

void loop() {
  WiFiClient client = server.available(); // Check for client connection
  
  if (client) {
    Serial.println("Client connected!");
    String request = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        
        // Break on empty line (end of HTTP request)
        if (request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }

    Serial.println("Received Data:");
    Serial.println(request);

    // Parse JSON Data
    int joyX = 0, joyY = 0, switchState = 0;
    if (parseJSON(request, joyX, joyY, switchState)) {
      Serial.print("Parsed Data - X: "); Serial.print(joyX);
      Serial.print(" | Y: "); Serial.print(joyY);
      Serial.print(" | Switch: "); Serial.println(switchState);

      // Control motors based on received joystick values
      controlMotors(joyX, joyY);
    } else {
      Serial.println("Failed to parse JSON!");
    }

    /* Send response to client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println("Data received!");
    
    client.stop();
    Serial.println("Client disconnected."); 
  }*/
  }
}

// Parse JSON from HTTP POST request
bool parseJSON(String request, int &joyX, int &joyY, int &switchState) {
  int jsonStart = request.indexOf('{');
  int jsonEnd = request.lastIndexOf('}');

  if (jsonStart == -1 || jsonEnd == -1) {
    return false;
  }

  String jsonData = request.substring(jsonStart, jsonEnd + 1);
  Serial.println("Extracted JSON: " + jsonData);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
    Serial.print("JSON Error: ");
    Serial.println(error.c_str());
    return false;
  }

  joyX = doc["joyX"] | 0;
  joyY = doc["joyY"] | 0;
  switchState = doc["switch"] | 0;
  return true;
}

// MOTOR CONTROL FUNCTIONS
void controlMotors(int xAxis, int yAxis) {
  int motorSpeedA = 0;
  int motorSpeedB = 0;

  if (xAxis > 135) {      // Forward
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

