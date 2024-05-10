#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#define SS_PIN 15
#define RST_PIN 16
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

const char* ssid = "SSID";
const char* password = "PASSWORD";
// bool detectionEnabled = false;
ESP8266WebServer server(5512); // 设置监听端口为 80
WiFiClient wifiClient; // 创建 WiFiClient 对象
void setup() {
 	Serial.begin(115200);
 	SPI.begin(); // Init SPI bus
 	rfid.PCD_Init(); // Init MFRC522
 	Serial.println();
 	Serial.print(F("Reader :"));
 	rfid.PCD_DumpVersionToSerial();
 	for (byte i = 0; i < 6; i++) {
 			key.keyByte[i] = 0xFF;
 	}
 	Serial.println();
 	Serial.println(F("This code scan the MIFARE Classic NUID."));
 	Serial.print(F("Using the following key:"));
 	printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  server.on("/IRControl", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String message = server.arg("plain");
      // Serial.println(message);
      if (message == "1") {
        // detectionEnabled = false;
        rfid.PCD_Init();  //RFID 重置
      }
      //  else if (message == "0") {
      //   detectionEnabled = true;
        
      // }
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
}
void loop() {
  server.handleClient();
 	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
 	if ( ! rfid.PICC_IsNewCardPresent())
 			return;
 	// Verify if the NUID has been readed
 	if ( ! rfid.PICC_ReadCardSerial())
 			return;
 	Serial.print(F("PICC type: "));
 	MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
 	Serial.println(rfid.PICC_GetTypeName(piccType));
 	// Check is the PICC of Classic MIFARE type
 	if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
 					piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
 					piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
 			Serial.println(F("Your tag is not of type MIFARE Classic."));
 			return;
 	}
 	if (rfid.uid.uidByte[0] != nuidPICC[0] ||
 					rfid.uid.uidByte[1] != nuidPICC[1] ||
 					rfid.uid.uidByte[2] != nuidPICC[2] ||
 					rfid.uid.uidByte[3] != nuidPICC[3] ) {
 			Serial.println(F("A new card has been detected."));
 			// Store NUID into nuidPICC array
 			for (byte i = 0; i < 4; i++) {
 					nuidPICC[i] = rfid.uid.uidByte[i];
 			}
 			Serial.println(F("The NUID tag is:"));
 			Serial.print(F("In hex: "));
 			printHex(rfid.uid.uidByte, rfid.uid.size);
 			Serial.println();
 			Serial.print(F("In dec: "));
 			printDec(rfid.uid.uidByte, rfid.uid.size);
 			Serial.println();
      sendPostRequest("Green");
       
 	}
 	else Serial.println(F("Card read previously."));
 	// Halt PICC
 	rfid.PICC_HaltA();
 	// Stop encryption on PCD
 	rfid.PCD_StopCrypto1();
  
}
/**
 		Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
 	for (byte i = 0; i < bufferSize; i++) {
 			Serial.print(buffer[i] < 0x10 ? " 0" : " ");
 			Serial.print(buffer[i], HEX);
 	}
}
/**
 		Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
 	for (byte i = 0; i < bufferSize; i++) {
 			Serial.print(buffer[i] < 0x10 ? " 0" : " ");
 			Serial.print(buffer[i], DEC);
 	}
}
void sendPostRequest(const String& message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, "http://192.168.0.103:1880/IRRequest"); // 使用新的 API
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"message\":\"" + message + "\"}");

    // if (httpResponseCode > 0) {
    //   String response = http.getString();
    //   // Serial.println(response);
    // } else {
    //   //Serial.print("Error on sending POST: ");
    //   //Serial.println(httpResponseCode);
    // }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}