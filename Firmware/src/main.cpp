// src/main.cpp
#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <NimBLEDevice.h>
#include "LingMouEngine.h"

// ==========================================
// 💡 硬件引脚定义
// ==========================================
static const int I2C_SDA = 18;
static const int I2C_SCL = 19;
static const int LCD_BL_PIN = 22;
static const int BTN_PIN = 9;  // 模式切换按钮，低电平触发

// ==========================================
// 🖥️ 显示对象
// ==========================================
TFT_eSPI tft;
TFT_eSprite eyeSprite(&tft);  // 128x64 虚拟画布，替代 SSD1306

// ==========================================
// 🔄 显示模式
// ==========================================
enum DisplayMode { MODE_EYE = 0,
                   MODE_INFO = 1 };
volatile DisplayMode currentMode = MODE_EYE;
volatile bool modeChanged = false;
unsigned long lastBtnTime = 0;
const unsigned long DEBOUNCE_MS = 300;

// ==========================================
// 🌡️ AHT20 传感器
// ==========================================
Adafruit_AHTX0 aht;
float temperature = 0.0f;
float humidity = 0.0f;
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 3000;

// ==========================================
// 🌐 Wi-Fi + NTP
// ==========================================
#define WIFI_SSID "your_ssid"
#define WIFI_PASSWORD "your_password"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 8 * 3600, 60000);  // UTC+8
String timeStr = "--:--:--";
unsigned long lastNtpUpdate = 0;
const unsigned long NTP_INTERVAL = 30000;

// ==========================================
// 📡 BLE（保留旧项目全部 UUID 和指令）
// ==========================================
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a9"  // 新增 Notify

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pNotifyChar = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;


// ==========================================
// 👁️ 眼睛引擎（照搬旧项目，一字未改）
// ==========================================
static Face* gFace = nullptr;
static uint32_t lastFrameMs = 0;
static const uint16_t EYES_FRAME_MS = 33;

unsigned long lastBleCmdTime = 0;
unsigned long lastScaredTime = 0;
float last_look_x = 0.0;
float last_look_y = 0.0;

Adafruit_MPU6050 mpu;

// ==========================================
// 🛠️ BLE 回调（照搬旧项目，加了断线重广播）
// ==========================================
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s) {
    deviceConnected = true;
    Serial.println("🔵 蓝牙已物理直连！");
  }
  void onDisconnect(NimBLEServer* s) {
    deviceConnected = false;
    Serial.println("🔴 蓝牙意外断开！");
    NimBLEDevice::startAdvertising();
  }
};

class MyCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      String msg = String(rxValue.c_str());
      Serial.print("【MTU指令击中】: ");
      Serial.println(msg);

      lastBleCmdTime = millis();

      if (msg == "B:1") {
        gFace->DoBlink();
      } else if (msg.startsWith("I:")) {
        gFace->RandomBehavior = (msg.substring(2).toInt() == 1);
        if (gFace->RandomBehavior) lastBleCmdTime = 0;
      } else if (msg.startsWith("W:")) {
        gFace->RandomLook = (msg.substring(2).toInt() == 1);
        if (gFace->RandomLook) lastBleCmdTime = 0;
      } else if (msg.startsWith("E:")) {
        int eIndex = msg.substring(2).toInt();
        if (eIndex >= 0 && eIndex < eEmotions::EMOTIONS_COUNT) {
          gFace->RandomBehavior = false;
          gFace->Behavior.GoToEmotion((eEmotions)eIndex);
        }
      } else if (msg.startsWith("X:")) {
        int commaIndex = msg.indexOf(',');
        if (commaIndex > 0) {
          float x = constrain(msg.substring(2, commaIndex).toFloat(), -1.0, 1.0);
          float y = constrain(msg.substring(commaIndex + 1).toFloat(), -1.0, 1.0);
          gFace->RandomLook = false;
          gFace->Look.LookAt(x, y);
        }
      }
    }
  }
};


// ==========================================
// 🔘 按钮中断
// ==========================================
void IRAM_ATTR onButtonPress() {
  unsigned long now = millis();
  if (now - lastBtnTime > DEBOUNCE_MS) {
    lastBtnTime = now;
    currentMode = (currentMode == MODE_EYE) ? MODE_INFO : MODE_EYE;
    modeChanged = true;
  }
}

// ==========================================
// 📊 Info 界面绘制
// ==========================================
void drawInfoScreen() {
  tft.fillScreen(TFT_BLACK);

  // 标题
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextFont(4);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("LingMou", 86, 10);

  // 分割线
  tft.drawFastHLine(10, 40, 152, TFT_DARKGREY);

  // 时间（大字体 HH:MM）
  tft.setTextFont(6);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(timeStr.substring(0, 5), 86, 50);

  // 温度
  tft.setTextFont(4);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Temp", 10, 130);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f C", temperature);
  tft.drawString(buf, 10, 158);

  // 湿度
  tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  tft.drawString("Humidity", 10, 200);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  snprintf(buf, sizeof(buf), "%.1f %%", humidity);
  tft.drawString(buf, 10, 228);

  // 分割线
  tft.drawFastHLine(10, 270, 152, TFT_DARKGREY);

  // 连接状态
  tft.setTextFont(2);
  tft.setTextColor(WiFi.status() == WL_CONNECTED ? TFT_GREEN : TFT_RED, TFT_BLACK);
  tft.drawString(WiFi.status() == WL_CONNECTED ? "WiFi: ON" : "WiFi: OFF", 10, 280);
  tft.setTextColor(deviceConnected ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
  tft.drawString(deviceConnected ? "BLE: Connected" : "BLE: Waiting", 10, 298);
}

// ==========================================
// 📡 发送 BLE 遥测（Info 模式数据推送）
// ==========================================
void sendBleTelemetry() {
  if (!deviceConnected || pNotifyChar == nullptr) return;
  char buf[96];
  snprintf(buf, sizeof(buf),
           "{\"t\":%.1f,\"h\":%.1f,\"time\":\"%s\",\"wifi\":%d}",
           temperature, humidity,
           timeStr.c_str(),
           (WiFi.status() == WL_CONNECTED) ? 1 : 0);
  pNotifyChar->setValue((uint8_t*)buf, strlen(buf));
  pNotifyChar->notify();
}

// ==========================================
// 🚀 系统初始化
// ==========================================
void setup() {
  Serial.begin(115200);

  // 背光（先亮屏）
  pinMode(LCD_BL_PIN, OUTPUT);
  digitalWrite(LCD_BL_PIN, HIGH);

  // LCD 初始化
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // 眼睛 Sprite：128x64 模拟 SSD1306 画布
  eyeSprite.createSprite(128, 64);
  eyeSprite.fillSprite(TFT_BLACK);

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  // MPU6050（照搬旧项目）
  if (!mpu.begin()) Serial.println("【致命警告】陀螺仪失联！");
  else {
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }

  // AHT20
  if (aht.begin()) Serial.println("AHT20 OK");
  else Serial.println("AHT20 FAIL");

  // Face 引擎（照搬旧项目）
  gFace = new Face(128, 64, 40);
  gFace->RandomBehavior = false;
  gFace->RandomLook = false;
  gFace->RandomBlink = true;
  gFace->Behavior.GoToEmotion(eEmotions::Normal);

  // 按钮
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), onButtonPress, FALLING);

  // BLE（保留旧 UUID，新增 Notify 特征）
  // BLE（NimBLE 版本）
  NimBLEDevice::init("LingMou");
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  NimBLEService* pService = pServer->createService(SERVICE_UUID);

  // 旧项目：指令接收特征
  NimBLECharacteristic* pCmdChar = pService->createCharacteristic(
    CHARACTERISTIC_UUID_RX,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
  );
  pCmdChar->setCallbacks(new MyCallbacks());

  // 新增：遥测推送特征
  pNotifyChar = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    NIMBLE_PROPERTY::NOTIFY
  );

  pService->start();
  NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->start();



  // Wi-Fi + NTP（放最后，避免阻塞前面初始化）
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting WiFi");
  unsigned long wStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wStart < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
    timeClient.begin();
    timeClient.update();
    timeStr = timeClient.getFormattedTime();
  } else {
    Serial.println("\nWiFi timeout，离线模式");
  }

  Serial.println("Setup done");
}

// ==========================================
// 🔄 主循环
// ==========================================
void loop() {
  unsigned long now = millis();

  // ── 断线重广播 ──────────────
  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    NimBLEDevice::startAdvertising();
    oldDeviceConnected = deviceConnected;
  }
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }


  // ── 模式切换：清屏避免残影 ──────────────────
  if (modeChanged) {
    modeChanged = false;
    tft.fillScreen(TFT_BLACK);
  }

  // ── 传感器定时读取 ───────────────────────────
  if (now - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = now;
    sensors_event_t humEvent, tempEvent;
    if (aht.getEvent(&humEvent, &tempEvent)) {
      temperature = tempEvent.temperature;
      humidity = humEvent.relative_humidity;
    }
    sendBleTelemetry();
  }

  // ── NTP 定时同步 ─────────────────────────────
  if (WiFi.status() == WL_CONNECTED && now - lastNtpUpdate >= NTP_INTERVAL) {
    lastNtpUpdate = now;
    if (timeClient.update()) {
      timeStr = timeClient.getFormattedTime();
    }
  }

  // ── 模式渲染 ─────────────────────────────────
  if (currentMode == MODE_EYE) {
    // 姿态检测（照搬旧项目，一字未改）
    sensors_event_t a, g, temp;
    if (gFace && mpu.getEvent(&a, &g, &temp)) {
      if (millis() - lastBleCmdTime > 5000) {
        float total_acc = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
        if (abs(total_acc - 9.8) > 6.0) lastScaredTime = millis();

        if (millis() - lastScaredTime < 1500) {
          if (gFace->Behavior.CurrentEmotion != eEmotions::Scared)
            gFace->Behavior.GoToEmotion(eEmotions::Scared);
        } else {
          if (gFace->Behavior.CurrentEmotion == eEmotions::Scared)
            gFace->Behavior.GoToEmotion(eEmotions::Normal);
        }

        if (!gFace->RandomLook && gFace->Behavior.CurrentEmotion != eEmotions::Scared) {
          float look_x = constrain(-a.acceleration.x / 4.0, -1.0, 1.0);
          float look_y = constrain(a.acceleration.y / 4.0, -1.0, 1.0);
          if (abs(look_x - last_look_x) > 0.15 || abs(look_y - last_look_y) > 0.15) {
            gFace->Look.LookAt(look_x, look_y);
            last_look_x = look_x;
            last_look_y = look_y;
          }
        }
      }
    }

    // 帧率控制（照搬旧项目）
    if (gFace && (uint32_t)(millis() - lastFrameMs) >= EYES_FRAME_MS) {
      gFace->Update();
      // Sprite 推到屏幕中央（172x320 屏，sprite 128x64）
      // x = (172-128)/2 = 22，y = (320-64)/2 = 128
      eyeSprite.pushSprite(22, 128);
      lastFrameMs = millis();
    }

  } else {
    // Info 模式：1 秒刷新一次
    drawInfoScreen();
    delay(1000);
  }
}
