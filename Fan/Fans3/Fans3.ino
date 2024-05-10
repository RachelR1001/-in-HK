#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <FastLED.h>

WebServer server(1650);
#define LED_PIN     23
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];


const char* ssid = "SSID";
const char* password = "PASSWORD";

const int airIn = 18; // 定义GPIO引脚号，可以根据需要更改
const int airOut = 19;

const int magnetHigh =2;
const int magnetLow =4;

String switchMsg;
bool ledSwitch = true;

void setup() {

  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("");
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());
  server.on("/fans_ready", HTTP_POST, handleRoot);
  server.begin();
  
  //马达
  pinMode(airIn, OUTPUT); // 将GPIO引脚设置为输出模式
  pinMode(airOut, OUTPUT);

  //电磁铁
  pinMode(magnetHigh, OUTPUT); // 将GPIO引脚设置为输出模式
  pinMode(magnetLow, OUTPUT);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
}
// 定义你自己的Post服务，response是可以替换掉的当别人向你请求时你需要Response给他的内容
void handleRoot() {
  StaticJsonDocument<200> doc;
  
  if (server.hasArg("plain")) {
    // 解析接收到的JSON数据
    deserializeJson(doc, server.arg("plain"));
      String message = server.arg("plain");
      switchMsg= message;
    
       // 获取请求体中的数据
      Serial.println(message); // 打印接收到的数据
  } else {
    // 如果没有收到数据，添加默认值
    doc["message"] = "No message received";
  }

  // 添加更多数据到JSON对象
  doc["response"] = "Message received";
  doc["timestamp"] = millis();

  // 序列化JSON并发送响应
  String response;


  serializeJson(doc, response);
  Serial.println(response);

  server.send(200, "application/json", response);
  

}

void loop() {
    HTTPClient http;
    server.handleClient();

  
    Serial.print("the value is 0");
    digitalWrite(airIn, LOW); // 设置引脚为高电平
    digitalWrite(airOut, LOW);

    digitalWrite(magnetHigh, HIGH);
    digitalWrite(magnetLow, LOW);

    if(switchMsg.toInt() ==0){
    delay(2000);
    if(ledSwitch){
    for (int i = 0; i <= 29; i++) {
    leds[i] = CRGB ( 255, 255, 255);
    FastLED.show();
    delay(100);
  }
  delay(5000);
  for (int i = 0; i <= 29; i++) {
  leds[i] = CRGB ( 0, 0, 0);
  }
  FastLED.show();
  ledSwitch = false;
  }
    
    
    }else if(switchMsg.toInt()==1){
    digitalWrite(magnetHigh, LOW);
    digitalWrite(magnetLow, LOW);

    Serial.println("the value is 1");
    digitalWrite(airIn, HIGH); // 设置引脚为高电平
    digitalWrite(airOut, LOW);
    Serial.println("充气中"); // 通过串口监视器打印状态信息
    delay(5000); // 等待五秒钟
    


    digitalWrite(airOut, HIGH);
    digitalWrite(airIn, LOW); // 设置引脚为低电平
    Serial.println("排气中"); // 通过串口监视器打印状态信息
    delay(2000); // 等待五秒钟
    
    ledSwitch= true;

    }
} 

