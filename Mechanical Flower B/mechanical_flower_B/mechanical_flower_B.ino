#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Stepper motor
#include <AccelStepper.h>
const int stepsPerRevolution = 2048;

// ULN2003 Motor Driver Pins
#define motor1IN1 23
#define motor1IN2 25
#define motor1IN3 26
#define motor1IN4 27

#define motor2IN1 32
#define motor2IN2 33
#define motor2IN3 18
#define motor2IN4 19

// Relay control pins
#define relayPin1 4
//#define relayPin2 13

// Initialize the stepper library
AccelStepper stepper1(AccelStepper::HALF4WIRE, motor1IN1, motor1IN3, motor1IN2, motor1IN4);
AccelStepper stepper2(AccelStepper::HALF4WIRE, motor2IN1, motor2IN3, motor2IN2, motor2IN4);

// WiFi connection
WebServer server(1990);
const char* ssid = "SSID";
const char* password = "PASSWORD";

String flower2Msg;
bool currentOn = true;  // Flag to track the relay state

void setup() {
  Serial.begin(9600);
  pinMode(relayPin1, OUTPUT);
  //pinMode(relayPin2, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("");
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());

  server.on("/flower2_ready", HTTP_POST, handleRoot);
  server.begin();

  // // Set the speed and acceleration
  stepper1.setMaxSpeed(1200);
  stepper1.setAcceleration(300);
  // Set target position
  stepper1.moveTo(1024);

  stepper2.setMaxSpeed(1200);
  stepper2.setAcceleration(300);
  // Set target position
  stepper2.moveTo(1024);
}

// Handle the "/flower2_ready" route
void handleRoot() {
  StaticJsonDocument<200> doc;
  
  if (server.hasArg("plain")) {
    // Parse the received JSON data
    deserializeJson(doc, server.arg("plain"));
    String message = server.arg("plain"); // Get the data from the request body;
    flower2Msg = message;
    Serial.println(message); // Print the received data
    Serial.println(flower2Msg);
  } else {
    // If no data received, add default value
    doc["message"] = "No message received";
  }

  // Add more data to the JSON object
  doc["response"] = "Message received";
  doc["timestamp"] = millis();

  // Serialize JSON and send response
  String response;
  serializeJson(doc, response);
  Serial.println(response);
  server.send(200, "application/json", response);
  // flower2Msg = response;
}

void loop() {
  HTTPClient http;
  server.handleClient();
  // int flower2Msg_value = flower2Msg.toInt();
  
  if (flower2Msg.toInt() == 1) {
    if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
      // Check if both motors have reached their target positions
      stepper1.setSpeed(500);
      stepper2.setSpeed(500);
      static int counter = 0;  // Counter to keep track of the loop iterations

      if (counter < 36) {  // Loop six times (three times clockwise and three times counterclockwise)
        if (counter % 2 == 0) {
          // Even iteration: rotate motors clockwise by 512 steps
          stepper1.moveTo(stepper1.currentPosition() + 2048);
          stepper2.moveTo(stepper2.currentPosition() + 2048);
        } else {
          // Odd iteration: rotate motors counterclockwise by 512 steps
          stepper1.moveTo(stepper1.currentPosition() - 2048);
          stepper2.moveTo(stepper2.currentPosition() - 2048);
        }
        counter++;
      } else {
        // Stop the motors after six iterations
        stepper1.stop();
        stepper2.stop();
        Serial.println("Motors stopped");
      }
      digitalWrite(relayPin1, HIGH);  // Turn on the relay
      
      currentOn = false;
      //Serial.println("Current is off");
      // // Control the relay state
      // if (!currentOn) {
      //   digitalWrite(relayPin1, HIGH);  // Turn on the relay
      
      //   currentOn = true;
      //   Serial.println("Current is ON");
      // } else {
      //   digitalWrite(relayPin1, LOW);  // Turn off the relay
        
      //   currentOn = false;
      //   Serial.println("Current is OFF");
      // }
    }

    // Move the stepper motors (one step at a time)
    stepper1.run();
    stepper2.run();
  }else if (flower2Msg.toInt() == 0) {
    digitalWrite(relayPin1, LOW);  // Turn off the relay
    currentOn = true;
    //Serial.println("Current is ON");
    if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
      // Check if both motors have reached their target positions
      stepper1.setSpeed(800);
      stepper2.setSpeed(800);
      static int counter = 0;  // Counter to keep track of the loop iterations

      if (counter < 36) {  // Loop six times (three times clockwise and three times counterclockwise)
        if (counter % 2 == 0) {
          // Even iteration: rotate motors clockwise by 512 steps
          stepper1.moveTo(stepper1.currentPosition() + 2048);
          stepper2.moveTo(stepper2.currentPosition() + 2048);
        } else {
          // Odd iteration: rotate motors counterclockwise by 512 steps
          stepper1.moveTo(stepper1.currentPosition() - 2048);
          stepper2.moveTo(stepper2.currentPosition() - 2048);
        }
        counter++;
      } else {
        // Stop the motors after six iterations
        stepper1.stop();
        stepper2.stop();
        Serial.println("Motors stopped");
      }
      
        
      
      // // Control the relay state
      // if (!currentOn) {
      //   digitalWrite(relayPin1, HIGH);  // Turn on the relay
      
      //   currentOn = true;
      //   Serial.println("Current is ON");
      // } else {
      //   digitalWrite(relayPin1, LOW);  // Turn off the relay
        
      //   currentOn = false;
      //   Serial.println("Current is OFF");
      // }
    }

    // Move the stepper motors (one step at a time)
    stepper1.run();
    stepper2.run();
  }
}