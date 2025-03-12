#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

// WiFi Credentials 
char ssid[] = "TP-Link_6373";      // WiFi network name
char pass[] = "haha_not_gonna_leak_it";  // WiFi password
int status = WL_IDLE_STATUS;

// Receiver's IP Address and Port
char server[] = "192.168.0.106";  
int port = 80;

// Pin Definitions
#define JOY_BTN 0    // Joystick button
#define OUTPUT_A 6    // Encoder Clock (A)
#define OUTPUT_B 7    // Encoder Data (B)
#define SWITCH 8      // Toggle switch

int counter = 0;
int aState;
int aLastState;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  
  // Sensor pin setup
  pinMode(JOY_BTN, INPUT_PULLUP);
  pinMode(OUTPUT_A, INPUT);
  pinMode(OUTPUT_B, INPUT);
  pinMode(SWITCH, INPUT);
  
  aLastState = digitalRead(OUTPUT_A);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read joystick values (map 0-1023 to 0-255)
  int joyX = map(analogRead(A1), 0, 1023, 0, 255);
  int joyY = map(analogRead(A2), 0, 1023, 0, 255);
  int switchState = digitalRead(SWITCH);

  // Read rotary encoder
  aState = digitalRead(OUTPUT_A);
  if (aState != aLastState) {  
    if (digitalRead(OUTPUT_B) != aState) {
      counter++;
    } else {
      counter--;
    }
  }
  aLastState = aState;

  // Create JSON payload
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["joyX"] = joyX;
  jsonDoc["joyY"] = joyY;
  jsonDoc["switch"] = switchState;
  jsonDoc["encoder"] = counter;
  
  String payload;
  serializeJson(jsonDoc, payload);

  Serial.println("Sending: " + payload);
  
  // Send data to receiver
  if (client.connect(server, port)) {
    client.println(payload);
    
    Serial.println("Data sent!");

    client.stop();
  } else {
    Serial.println("Connection to receiver failed!");
  }

  delay(500); // Adjust based on required update rate
}
