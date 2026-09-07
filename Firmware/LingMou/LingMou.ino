#include <Wire.h>
#include <TFT_eSPI.h>
#include <Arduino_GFX_Library.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <NTPClient.h>
#include <Preferences.h>
#include <WebServer.h>
#include <NimBLEDevice.h>
#include "HWCDC.h"
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
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
HWCDC DebugUSB;

// TFT_eSPI 只用作内存 Sprite，不再直接控制 LCD
TFT_eSPI spriteHost;
TFT_eSprite eyeSprite(&spriteHost);

// Waveshare ESP32-C6-LCD-1.47
static const int LCD_MOSI_PIN = 6;
static const int LCD_SCLK_PIN = 7;
static const int LCD_CS_PIN   = 14;
static const int LCD_DC_PIN   = 15;
static const int LCD_RST_PIN  = 21;
static const int SD_CS_PIN    = 4;

// Arduino_GFX 负责真正的 LCD
Arduino_DataBus *lcdBus = new Arduino_ESP32SPI(
  LCD_DC_PIN,
  LCD_CS_PIN,
  LCD_SCLK_PIN,
  LCD_MOSI_PIN,
  GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_ST7789(
  lcdBus,
  LCD_RST_PIN,
  0,       // rotation
  true,    // IPS
  172,
  320,
  34,      // X offset
  0,
  34,
  0
);

// RGB565
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_CYAN      0x07FF
#define COLOR_YELLOW    0xFFE0
#define COLOR_ORANGE    0xFD20
#define COLOR_SKYBLUE   0x867D
#define COLOR_DARKGREY  0x7BEF
#define COLOR_PANEL     0x0841   // 深蓝黑卡片背景

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
bool ahtReady = false;     // AHT20 是否初始化成功
float temperature = 0.0f;
float humidity = 0.0f;
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 3000;

// ==========================================
// 🌐 Wi-Fi + NTP + App 局域网接口
// ==========================================
// 首次配网时，App 通过 BLE 发送：
//   WIFI_SSID=<ssid>
//   WIFI_PASS=<password>
//   WIFI_CONNECT
// 若字段超过手机默认 BLE 单包长度，可先发 WIFI_SSID= / WIFI_PASS=，
// 后续分片使用 WIFI_SSID+ / WIFI_PASS+ 追加，再发送 WIFI_CONNECT。
// 凭据会保存到 ESP32-C6 的 NVS，重启后自动重连。
#ifndef LINGMOU_DEFAULT_WIFI_SSID
#define LINGMOU_DEFAULT_WIFI_SSID ""
#endif
#ifndef LINGMOU_DEFAULT_WIFI_PASSWORD
#define LINGMOU_DEFAULT_WIFI_PASSWORD ""
#endif

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 8 * 3600, 60000);  // UTC+8
String timeStr = "--:--:--";
String dateStr = "---- -- --";
String weekdayStr = "---";

Preferences wifiPreferences;
WebServer appServer(80);
String wifiSsid;
String wifiPassword;
bool wifiConnecting = false;
bool appServerStarted = false;
bool timeClientStarted = false;
bool systemTimeValid = false;
unsigned long wifiConnectStarted = 0;
unsigned long nextWifiAttempt = 0;
unsigned long lastClockUpdate = 0;
const unsigned long WIFI_CONNECT_TIMEOUT = 15000;
const unsigned long WIFI_RETRY_INTERVAL = 30000;

unsigned long lastNtpUpdate = 0;
const unsigned long NTP_INTERVAL = 30000;

bool infoLayoutDirty = true;
unsigned long lastInfoDraw = 0;
const unsigned long INFO_REDRAW_MS = 1000;


// ==========================================
// 📡 BLE（保留旧项目全部 UUID 和指令）
// ==========================================
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TX "beb5483e-36e1-4688-b7f5-ea07361b26a9"  // App 订阅此特征接收遥测

NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pNotifyChar = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// BLE 回调会调用这些函数，提前声明以保持 Arduino 单文件编译兼容。
void handleBleCommand(const String& msg);
void sendBleMessage(const String& message);
void startWifiConnection(bool immediate);
void serviceWiFi();
void setupAppServer();
String buildTelemetryJson();
void updateDisplayTimeFromUtcEpoch(time_t utcEpoch);


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
bool mpuReady = false;     // MPU6050 是否初始化成功
// ==========================================
// 🛠️ BLE 回调（照搬旧项目，加了断线重广播）
// ==========================================
class MyServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s) {
    deviceConnected = true;
    DebugUSB.println("🔵 蓝牙已物理直连！");

  }
  void onDisconnect(NimBLEServer* s) {
    deviceConnected = false;
    DebugUSB.println("🔴 蓝牙意外断开！");
    NimBLEDevice::startAdvertising();
  }
};

class MyCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
      String msg = String(rxValue.c_str());
      msg.trim();
      DebugUSB.print("【BLE指令】: ");
      if (msg.startsWith("WIFI_PASS=") || msg.startsWith("WIFI_PASSWORD=") ||
          msg.startsWith("WIFI_PASS+") || msg.startsWith("WIFI_PASSWORD+")) {
        DebugUSB.println("<hidden>");
      } else {
        DebugUSB.println(msg);
      }
      handleBleCommand(msg);
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
// ==========================================
// 📊 Info 界面：新版仪表盘
// ==========================================
void updateDateStrings() {
  // NTPClient 已经设置 UTC+8 偏移，因此这里用 gmtime_r 读取调整后的 epoch。
  if (WiFi.status() != WL_CONNECTED) return;

  time_t localEpoch = (time_t)timeClient.getEpochTime();
  struct tm tmInfo;
  gmtime_r(&localEpoch, &tmInfo);

  char dateBuf[16];
  snprintf(
    dateBuf,
    sizeof(dateBuf),
    "%04d-%02d-%02d",
    tmInfo.tm_year + 1900,
    tmInfo.tm_mon + 1,
    tmInfo.tm_mday
  );
  dateStr = dateBuf;

  static const char* WEEKDAYS[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
  };
  weekdayStr = WEEKDAYS[tmInfo.tm_wday];
}

void updateDisplayTimeFromUtcEpoch(time_t utcEpoch) {
  time_t localEpoch = utcEpoch + 8 * 3600;
  struct tm tmInfo;
  gmtime_r(&localEpoch, &tmInfo);

  char timeBuf[12];
  char dateBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", tmInfo.tm_hour, tmInfo.tm_min, tmInfo.tm_sec);
  snprintf(dateBuf, sizeof(dateBuf), "%04d-%02d-%02d", tmInfo.tm_year + 1900, tmInfo.tm_mon + 1, tmInfo.tm_mday);
  timeStr = timeBuf;
  dateStr = dateBuf;

  static const char* WEEKDAYS[] = {
    "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
  };
  weekdayStr = WEEKDAYS[tmInfo.tm_wday];
}

String jsonEscape(const String& value) {
  String escaped;
  escaped.reserve(value.length() + 8);
  for (size_t i = 0; i < value.length(); ++i) {
    const char c = value[i];
    if (c == '\\' || c == '"') {
      escaped += '\\';
    }
    escaped += c;
  }
  return escaped;
}

String buildTelemetryJson() {
  char buf[160];
  const int wifiConnected = (WiFi.status() == WL_CONNECTED) ? 1 : 0;

  if (ahtReady) {
    snprintf(
      buf,
      sizeof(buf),
      "{\"t\":%.1f,\"h\":%.1f,\"aht\":1,\"time\":\"%s\",\"wifi\":%d}",
      temperature,
      humidity,
      timeStr.c_str(),
      wifiConnected
    );
  } else {
    snprintf(
      buf,
      sizeof(buf),
      "{\"t\":null,\"h\":null,\"aht\":0,\"time\":\"%s\",\"wifi\":%d}",
      timeStr.c_str(),
      wifiConnected
    );
  }

  return String(buf);
}

String buildStatusJson() {
  const bool connected = (WiFi.status() == WL_CONNECTED);
  String json = "{\"wifi\":";
  json += connected ? "1" : "0";
  json += ",\"connecting\":";
  json += wifiConnecting ? "1" : "0";
  json += ",\"ble\":";
  json += deviceConnected ? "1" : "0";
  json += ",\"ssid\":\"";
  json += jsonEscape(wifiSsid);
  json += "\",\"ip\":\"";
  json += connected ? WiFi.localIP().toString() : "";
  json += "\",\"telemetry\":";
  json += buildTelemetryJson();
  json += "}";
  return json;
}

void sendBleMessage(const String& message) {
  if (!deviceConnected || pNotifyChar == nullptr) return;
  pNotifyChar->setValue((uint8_t*)message.c_str(), message.length());
  pNotifyChar->notify();
}

void loadWifiCredentials() {
  wifiPreferences.begin("wifi", true);
  wifiSsid = wifiPreferences.getString("ssid", LINGMOU_DEFAULT_WIFI_SSID);
  wifiPassword = wifiPreferences.getString("password", LINGMOU_DEFAULT_WIFI_PASSWORD);
  wifiPreferences.end();
  wifiSsid.trim();
}

void saveWifiCredentials() {
  wifiPreferences.begin("wifi", false);
  wifiPreferences.putString("ssid", wifiSsid);
  wifiPreferences.putString("password", wifiPassword);
  wifiPreferences.end();
}

void clearWifiCredentials() {
  wifiPreferences.begin("wifi", false);
  wifiPreferences.clear();
  wifiPreferences.end();
  wifiSsid = "";
  wifiPassword = "";
  wifiConnecting = false;
  nextWifiAttempt = 0;
  WiFi.disconnect();
}

void startWifiConnection(bool immediate) {
  if (wifiSsid.length() == 0) {
    wifiConnecting = false;
    sendBleMessage("{\"type\":\"wifi\",\"ok\":0,\"error\":\"ssid_empty\"}");
    return;
  }

  if (!immediate && millis() < nextWifiAttempt) return;
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
  wifiConnecting = true;
  wifiConnectStarted = millis();
  nextWifiAttempt = wifiConnectStarted + WIFI_RETRY_INTERVAL;
  DebugUSB.println("WiFi connection started");
}

void handleApiStatus() {
  appServer.sendHeader("Access-Control-Allow-Origin", "*");
  appServer.send(200, "application/json", buildStatusJson());
}

void handleApiTelemetry() {
  appServer.sendHeader("Access-Control-Allow-Origin", "*");
  appServer.send(200, "application/json", buildTelemetryJson());
}

void setupAppServer() {
  if (appServerStarted) return;

  appServer.on("/", HTTP_GET, []() {
    appServer.sendHeader("Access-Control-Allow-Origin", "*");
    appServer.send(200, "text/plain", "LingMou API: /api/status or /api/telemetry");
  });
  appServer.on("/api/status", HTTP_GET, handleApiStatus);
  appServer.on("/api/telemetry", HTTP_GET, handleApiTelemetry);
  appServer.onNotFound([]() {
    appServer.sendHeader("Access-Control-Allow-Origin", "*");
    appServer.send(404, "application/json", "{\"error\":\"not_found\"}");
  });
  appServer.begin();
  appServerStarted = true;
  DebugUSB.println("App HTTP API started on port 80");
}

void serviceWiFi() {
  const wl_status_t status = WiFi.status();

  if (status == WL_CONNECTED) {
    if (wifiConnecting || !timeClientStarted) {
      wifiConnecting = false;
      if (!timeClientStarted) {
        timeClient.begin();
        timeClientStarted = true;
      }
      timeClient.update();
      timeStr = timeClient.getFormattedTime();
      updateDateStrings();
      setupAppServer();
      DebugUSB.println("WiFi connected: " + WiFi.localIP().toString());
      sendBleMessage("{\"type\":\"wifi\",\"ok\":1,\"ip\":\"" + WiFi.localIP().toString() + "\"}");
    }

    if (appServerStarted) appServer.handleClient();
    return;
  }

  if (wifiConnecting && millis() - wifiConnectStarted >= WIFI_CONNECT_TIMEOUT) {
    wifiConnecting = false;
    WiFi.disconnect();
    nextWifiAttempt = millis() + WIFI_RETRY_INTERVAL;
    DebugUSB.println("WiFi connection timeout");
    sendBleMessage("{\"type\":\"wifi\",\"ok\":0,\"error\":\"timeout\"}");
  }

  if (!wifiConnecting && wifiSsid.length() > 0 && millis() >= nextWifiAttempt) {
    startWifiConnection(false);
  }
}

void handleBleCommand(const String& msg) {
  lastBleCmdTime = millis();

  if (msg.startsWith("WIFI_SSID=")) {
    wifiSsid = msg.substring(strlen("WIFI_SSID="));
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"ssid\",\"ok\":1}");
    return;
  }

  if (msg.startsWith("WIFI_SSID+")) {
    wifiSsid += msg.substring(strlen("WIFI_SSID+"));
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"ssid\",\"ok\":1}");
    return;
  }

  if (msg.startsWith("WIFI_PASS=")) {
    wifiPassword = msg.substring(strlen("WIFI_PASS="));
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"password\",\"ok\":1}");
    return;
  }

  if (msg.startsWith("WIFI_PASS+")) {
    wifiPassword += msg.substring(strlen("WIFI_PASS+"));
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"password\",\"ok\":1}");
    return;
  }

  if (msg.startsWith("WIFI_PASSWORD+")) {
    wifiPassword += msg.substring(strlen("WIFI_PASSWORD+"));
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"password\",\"ok\":1}");
    return;
  }

  if (msg.startsWith("WIFI_PASSWORD=")) {
    const int separator = msg.indexOf('=');
    wifiPassword = msg.substring(separator + 1);
    sendBleMessage("{\"type\":\"wifi_config\",\"field\":\"password\",\"ok\":1}");
    return;
  }

  if (msg == "WIFI_CONNECT") {
    if (wifiSsid.length() == 0) {
      sendBleMessage("{\"type\":\"wifi\",\"ok\":0,\"error\":\"ssid_empty\"}");
      return;
    }
    saveWifiCredentials();
    nextWifiAttempt = 0;
    startWifiConnection(true);
    sendBleMessage("{\"type\":\"wifi\",\"ok\":1,\"state\":\"connecting\"}");
    return;
  }

  if (msg == "WIFI_CLEAR") {
    clearWifiCredentials();
    sendBleMessage("{\"type\":\"wifi\",\"ok\":1,\"state\":\"cleared\"}");
    return;
  }

  if (msg == "WIFI_STATUS" || msg == "GET_STATUS") {
    sendBleMessage(buildStatusJson());
    return;
  }

  if (msg == "GET_TELEMETRY") {
    sendBleMessage(buildTelemetryJson());
    return;
  }

  if (msg.startsWith("TIME=")) {
    const long long epoch = atoll(msg.substring(strlen("TIME=")).c_str());
    if (epoch > 0) {
      struct timeval tv;
      tv.tv_sec = (time_t)epoch;
      tv.tv_usec = 0;
      settimeofday(&tv, nullptr);
      systemTimeValid = true;
      updateDisplayTimeFromUtcEpoch((time_t)epoch);
      sendBleMessage("{\"type\":\"time\",\"ok\":1}");
    } else {
      sendBleMessage("{\"type\":\"time\",\"ok\":0}");
    }
    return;
  }

  if (msg == "B:1") {
    if (gFace) gFace->DoBlink();
  } else if (msg.startsWith("I:")) {
    if (gFace) {
      gFace->RandomBehavior = (msg.substring(2).toInt() == 1);
      if (gFace->RandomBehavior) lastBleCmdTime = 0;
    }
  } else if (msg.startsWith("W:")) {
    if (gFace) {
      gFace->RandomLook = (msg.substring(2).toInt() == 1);
      if (gFace->RandomLook) lastBleCmdTime = 0;
    }
  } else if (msg.startsWith("E:")) {
    int eIndex = msg.substring(2).toInt();
    if (gFace && eIndex >= 0 && eIndex < eEmotions::EMOTIONS_COUNT) {
      gFace->RandomBehavior = false;
      gFace->Behavior.GoToEmotion((eEmotions)eIndex);
    }
  } else if (msg.startsWith("X:")) {
    int commaIndex = msg.indexOf(',');
    if (gFace && commaIndex > 0) {
      float x = constrain(msg.substring(2, commaIndex).toFloat(), -1.0, 1.0);
      float y = constrain(msg.substring(commaIndex + 1).toFloat(), -1.0, 1.0);
      gFace->RandomLook = false;
      gFace->Look.LookAt(x, y);
    }
  }
}

void drawThermometerIcon(int x, int y) {
  // 约 22x38 的温度计图标
  gfx->drawRoundRect(x + 6, y, 10, 27, 4, COLOR_WHITE);
  gfx->fillRect(x + 9, y + 6, 4, 19, COLOR_ORANGE);
  gfx->drawCircle(x + 11, y + 29, 8, COLOR_WHITE);
  gfx->fillCircle(x + 11, y + 29, 5, COLOR_ORANGE);
}

void drawDropIcon(int x, int y) {
  // 简单水滴轮廓
  gfx->drawLine(x + 11, y, x + 2, y + 15, COLOR_CYAN);
  gfx->drawLine(x + 11, y, x + 20, y + 15, COLOR_CYAN);
  gfx->drawLine(x + 2, y + 15, x + 2, y + 23, COLOR_CYAN);
  gfx->drawLine(x + 20, y + 15, x + 20, y + 23, COLOR_CYAN);
  gfx->drawLine(x + 2, y + 23, x + 7, y + 30, COLOR_CYAN);
  gfx->drawLine(x + 20, y + 23, x + 15, y + 30, COLOR_CYAN);
  gfx->drawFastHLine(x + 7, y + 30, 9, COLOR_CYAN);
  gfx->fillCircle(x + 11, y + 21, 6, COLOR_BLUE);
}

void drawWiFiIcon(int x, int y, uint16_t color) {
  // 14x12 Wi-Fi 图标
  gfx->drawLine(x, y + 4, x + 7, y, color);
  gfx->drawLine(x + 7, y, x + 14, y + 4, color);
  gfx->drawLine(x + 3, y + 7, x + 7, y + 4, color);
  gfx->drawLine(x + 7, y + 4, x + 11, y + 7, color);
  gfx->fillCircle(x + 7, y + 11, 2, color);
}

void drawBleIcon(int x, int y, uint16_t color) {
  // 极简 Bluetooth 标志
  gfx->drawFastVLine(x + 7, y, 16, color);
  gfx->drawLine(x + 7, y, x + 12, y + 5, color);
  gfx->drawLine(x + 12, y + 5, x + 3, y + 12, color);
  gfx->drawLine(x + 3, y + 4, x + 12, y + 11, color);
  gfx->drawLine(x + 12, y + 11, x + 7, y + 16, color);
}


void drawInfoStaticLayout() {
  gfx->fillScreen(COLOR_BLACK);

  // 标题
  gfx->setTextColor(COLOR_CYAN, COLOR_BLACK);
  gfx->setTextSize(2);
  gfx->setCursor(44, 8);
  gfx->println("LingMou");
  gfx->drawFastHLine(10, 34, 152, COLOR_CYAN);

  // 温度卡片：向下移动，给放大的日期留出空间
  gfx->fillRoundRect(8, 108, 156, 70, 9, COLOR_PANEL);
  gfx->drawRoundRect(8, 108, 156, 70, 9, COLOR_ORANGE);
  drawThermometerIcon(16, 127);

  gfx->setTextColor(COLOR_ORANGE, COLOR_PANEL);
  gfx->setTextSize(2);
  gfx->setCursor(52, 118);
  gfx->println("Temp");

  // 湿度卡片
  gfx->fillRoundRect(8, 186, 156, 70, 9, COLOR_PANEL);
  gfx->drawRoundRect(8, 186, 156, 70, 9, COLOR_CYAN);
  drawDropIcon(16, 205);

  gfx->setTextColor(COLOR_CYAN, COLOR_PANEL);
  gfx->setTextSize(2);
  gfx->setCursor(52, 196);
  gfx->println("Humidity");

  // 底部状态区
  gfx->drawFastHLine(10, 262, 152, COLOR_CYAN);
}

void drawInfoScreen() {
  if (infoLayoutDirty) {
    drawInfoStaticLayout();
    infoLayoutDirty = false;
  }

  char buf[32];

  // 时间
  gfx->fillRect(18, 42, 136, 42, COLOR_BLACK);
  gfx->setTextColor(COLOR_WHITE, COLOR_BLACK);
  gfx->setTextSize(4);
  gfx->setCursor(26, 44);
  gfx->println(timeStr.substring(0, 5));

  // 日期和星期：均为 2 号字，但分开显示
  gfx->fillRect(4, 82, 164, 22, COLOR_BLACK);
  gfx->setTextColor(COLOR_SKYBLUE, COLOR_BLACK);
  gfx->setTextSize(2);
  gfx->setCursor(4, 86);
  gfx->print(dateStr);

  gfx->setCursor(126, 86);
  gfx->print(weekdayStr);


  // 温度
  gfx->fillRect(50, 142, 108, 28, COLOR_PANEL);
  gfx->setTextColor(COLOR_WHITE, COLOR_PANEL);
  gfx->setTextSize(3);
  if (ahtReady) {
    snprintf(buf, sizeof(buf), "%.1f C", temperature);
  } else {
    snprintf(buf, sizeof(buf), "--.- C");
  }
  gfx->setCursor(52, 142);
  gfx->println(buf);

  // 湿度
  gfx->fillRect(50, 220, 108, 28, COLOR_PANEL);
  gfx->setTextColor(COLOR_WHITE, COLOR_PANEL);
  gfx->setTextSize(3);
  if (ahtReady) {
    snprintf(buf, sizeof(buf), "%.1f %%", humidity);
  } else {
    snprintf(buf, sizeof(buf), "--.- %%");
  }
  gfx->setCursor(52, 220);
  gfx->println(buf);

  // Wi-Fi
  bool wifiOn = (WiFi.status() == WL_CONNECTED);
  gfx->fillRect(10, 268, 152, 20, COLOR_BLACK);
  drawWiFiIcon(14, 271, wifiOn ? COLOR_GREEN : COLOR_DARKGREY);
  gfx->setTextSize(2);
  gfx->setTextColor(COLOR_SKYBLUE, COLOR_BLACK);
  gfx->setCursor(38, 269);
  gfx->print("WiFi");
  gfx->setTextColor(wifiOn ? COLOR_GREEN : COLOR_RED, COLOR_BLACK);
  gfx->setCursor(112, 269);
  gfx->print(wifiOn ? "ON" : "OFF");

  // BLE
  gfx->fillRect(10, 293, 152, 20, COLOR_BLACK);
  drawBleIcon(14, 294, deviceConnected ? COLOR_CYAN : COLOR_DARKGREY);
  gfx->setTextColor(COLOR_SKYBLUE, COLOR_BLACK);
  gfx->setCursor(38, 293);
  gfx->print("BLE");
  gfx->setTextColor(deviceConnected ? COLOR_GREEN : COLOR_YELLOW, COLOR_BLACK);
  gfx->setCursor(100, 293);
  gfx->print(deviceConnected ? "OK" : "WAIT");
}


// ==========================================
// 📡 发送 BLE 遥测（Info 模式数据推送）
// ==========================================
void sendBleTelemetry() {
  if (!deviceConnected || pNotifyChar == nullptr) return;
  sendBleMessage(buildTelemetryJson());
}


// ==========================================
// 🚀 系统初始化
// ==========================================
void setup() {
  DebugUSB.begin();
  delay(500);

  DebugUSB.println();
  DebugUSB.println("======================");
  DebugUSB.println("LingMou booting...");
  DebugUSB.println("======================");

  // 背光先关闭
  pinMode(LCD_BL_PIN, OUTPUT);
  analogWrite(LCD_BL_PIN, 0);

  // SD 与 LCD 共用 SPI，先禁止 SD
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  // 初始化真正的 ST7789
  DebugUSB.println("Initializing Arduino_GFX LCD...");

  if (!gfx->begin()) {
    DebugUSB.println("ERROR: gfx->begin() FAILED!");

    while (true) {
      delay(1000);
    }
  }

  // 清成黑屏
  gfx->fillScreen(COLOR_BLACK);

  // 背光约 39%
  analogWrite(LCD_BL_PIN, 100);

  DebugUSB.printf(
    "LCD initialized: %d x %d\r\n",
    gfx->width(),
    gfx->height()
  );

  // ==========================================
  // 128x64 眼睛内存 Sprite
  // ==========================================
  eyeSprite.setColorDepth(16);

  if (eyeSprite.createSprite(128, 64) == nullptr) {
    DebugUSB.println("ERROR: Eye sprite allocation FAILED!");

    while (true) {
      delay(1000);
    }
  }

  eyeSprite.fillSprite(TFT_BLACK);

  DebugUSB.println("Eye sprite OK");


  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(100000);

  // MPU6050
  mpuReady = mpu.begin();

  if (!mpuReady) {
    DebugUSB.println("MPU6050 FAIL - 姿态功能已禁用");
  } else {
    DebugUSB.println("MPU6050 OK");

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }


  // AHT20
  ahtReady = aht.begin();

  if (ahtReady) {
    DebugUSB.println("AHT20 OK");
  } else {
    DebugUSB.println("AHT20 FAIL - 温湿度功能已禁用");
  }


  // Face 引擎
  gFace = new Face(128, 64, 40);

  gFace->LeftEye.ApplyPreset(Preset_Normal);
  gFace->RightEye.ApplyPreset(Preset_Normal);

  gFace->RandomBehavior = false;
  gFace->RandomLook = false;
  gFace->RandomBlink = true;

  gFace->Behavior.GoToEmotion(eEmotions::Normal);

  DebugUSB.println("Face engine OK");


  // 按钮
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), onButtonPress, FALLING);

  // BLE（保留旧 UUID，新增 Notify 特征）
  // BLE（NimBLE 版本）
  NimBLEDevice::init("LingMou");
  NimBLEDevice::setMTU(247);
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
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  String initialTelemetry = buildTelemetryJson();
  pNotifyChar->setValue((uint8_t*)initialTelemetry.c_str(), initialTelemetry.length());

  pService->start();
  NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->start();



  // Wi-Fi + NTP：从 NVS 读取配网信息，非阻塞连接；BLE 始终保持可用。
  loadWifiCredentials();
  if (wifiSsid.length() > 0) {
    startWifiConnection(true);
  } else {
    DebugUSB.println("No WiFi credentials; waiting for BLE provisioning");
  }

  DebugUSB.println("Setup done");
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
    gfx->fillScreen(COLOR_BLACK);

    if (currentMode == MODE_INFO) {
      infoLayoutDirty = true;
      lastInfoDraw = 0;
    }
  }


// ── AHT20 定时读取 ───────────────────────────
if (now - lastSensorRead >= SENSOR_INTERVAL) {
  lastSensorRead = now;

  // 只有初始化成功以后才访问 AHT20
  if (ahtReady) {
    sensors_event_t humEvent, tempEvent;

    if (aht.getEvent(&humEvent, &tempEvent)) {
      temperature = tempEvent.temperature;
      humidity = humEvent.relative_humidity;
    }
  }

  sendBleTelemetry();
}

  // ── Wi-Fi 状态、HTTP API 与自动重连 ────────────
  serviceWiFi();

  // ── NTP 定时同步 ─────────────────────────────
  if (timeClientStarted && WiFi.status() == WL_CONNECTED && now - lastNtpUpdate >= NTP_INTERVAL) {
    lastNtpUpdate = now;
    if (timeClient.update()) {
      timeStr = timeClient.getFormattedTime();
      updateDateStrings();
    }
  }

  // 手机通过 BLE 授时后，让设备在本次上电期间继续走时。
  if (systemTimeValid && now - lastClockUpdate >= 1000) {
    lastClockUpdate = now;
    updateDisplayTimeFromUtcEpoch(time(nullptr));
  }

  // ── 模式渲染 ─────────────────────────────────
  if (currentMode == MODE_EYE) {
    // 姿态检测（照搬旧项目，一字未改）
  sensors_event_t a, g, temp;
  if (gFace && mpuReady && mpu.getEvent(&a, &g, &temp)) {
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

      // 把 TFT_eSPI 的内存 Sprite 交给 Arduino_GFX 输出
      gfx->draw16bitRGBBitmap(
        22,
        128,
        (uint16_t *)eyeSprite.getPointer(),
        128,
        64
      );

      lastFrameMs = millis();
    }


  } else {
    // Info 模式：非阻塞 1 秒刷新一次
    if (infoLayoutDirty || now - lastInfoDraw >= INFO_REDRAW_MS) {
      lastInfoDraw = now;
      drawInfoScreen();
    }
  }

}
