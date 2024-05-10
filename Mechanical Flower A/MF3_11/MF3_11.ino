#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Stepper.h>

// 步进电机控制引脚
#define IN1 18
#define IN2 19
#define IN3 21
#define IN4 22

WebServer server(1019);

const char* ssid = "SSID";
const char* password = "PASSWORD";
int receiveMsg = 2;

// 步进电机实例
Stepper stepper(2048, IN1, IN3, IN2, IN4); // 步进电机有2048个步进

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
  server.on("/flowerJ_ready", HTTP_POST, handleRoot);
  server.begin();

  // 初始化步进电机设置
  stepper.setSpeed(10); // 设置步进电机速度（步/秒），这里设置为慢速转动
  // 设置步进电机初始位置
  stepper.step(1024); // 旋转2048步，将步进电机转到初始位置
  delay(1000); // 停顿3秒
}

// 定义你自己的Post服务，response是可以替换掉的当别人向你请求时你需要Response给他的内容
void handleRoot() {
  if (server.hasArg("plain")) {
    String message = server.arg("plain"); // 获取请求体中的数据
    receiveMsg = message.toInt();
    Serial.println(message); // 打印接收到的数据
  } else {
    receiveMsg = 2; // 如果没有收到数据，将 receiveMsg 清空
  }

  // 创建 JSON 对象
  DynamicJsonDocument doc(200);
  doc["response"] = "Message received";
  doc["timestamp"] = millis();

  // 序列化 JSON 并发送响应
  String response;
  serializeJson(doc, response);
  Serial.println(response);
  server.send(200, "application/json", response);
}

void loop() {
  server.handleClient();

  if (receiveMsg == 1) { // 接收到消息为1时
    stepper.setSpeed(0); // 设置步进电机速度为0，停止转动
  }

  if (receiveMsg == 0) { // 接收到消息为0时
    for (int i = 0; i < 3; i++) {
      stepper.setSpeed(10);
      stepper.step(2 * 2048); // 旋转2 * 2048步，控制步进电机顺时针转动两圈
      delay(1000); // 停顿2秒
      // 逆时针旋转两圈
      stepper.step(-2 * 2048); // 旋转-2 * 2048步，控制步进电机逆时针转动两圈
      delay(1000); // 停顿2秒
      server.handleClient(); // 处理客户端请求，以便接收新的消息
    
    }
    }

  receiveMsg = 2;
}