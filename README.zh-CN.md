# 灵眸 LingMou — 基于多模态感知的随身智能交互挂件（面包板测试版）

[English](README.md) | [简体中文](README.zh-CN.md)

<p align="center">
  <img src="docs/images/hardware-eye-mode.jpg" width="800" alt="灵眸硬件原型 — 眼睛模式">
</p>

<p align="center">
  <b>一个能感知世界、并用情感眼睛回应的随身智能交互挂件。</b><br>
  <i>让机器拥有会回应世界的眼睛。</i>
</p>

<p align="center">
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License: MIT"></a>
  <img src="https://img.shields.io/badge/platform-ESP32--C6-E7352C.svg" alt="Platform: ESP32-C6">
  <img src="https://img.shields.io/badge/framework-Arduino-00979D.svg" alt="Framework: Arduino">
  <img src="https://img.shields.io/badge/app-Uni--app%20%2F%20Vue-42B883.svg" alt="App: Uni-app / Vue">
  <img src="https://img.shields.io/badge/status-Breadboard%20Prototype-orange.svg" alt="Status: Breadboard Prototype">
</p>

## 概述

**灵眸 (LingMou)** 是一个以 **ESP32-C6** 和 **1.47 寸 ST7789 LCD** 为核心的低成本、轻量化**基于多模态感知的随身智能交互挂件**。它把姿态感知（MPU6050）、环境感知（AHT20）、BLE + WiFi 双通道通信，以及 Uni-app 移动端控制器融合到一个随身小挂件里——不只是"显示"信息，更能**感知、决策、表达、响应**。

> **版本状态：面包板测试版。** 当前版本为**面包板测试版**，用于功能验证与快速迭代，**尚未**进行正式 PCB 设计；正式的 PCB 版图与外壳设计将在量产版本中完成。

倾斜它、摇晃它、或通过 App 与它交互：LCD 上的眼睛会实时地眨眼、凝视、受惊，并在 **18 种情绪**之间切换。

> **一句话定义：** 一个基于动画眼睛显示屏、板载 IMU/环境传感器和 BLE/WiFi 双通道通信，实现完整 *感知 → 决策 → 表达 → 反馈* 闭环的随身智能交互挂件。
>
> **项目口号：** *让机器拥有会回应世界的眼睛。*

本项目源自大学小学期综合能力实训小组课题，在此以开源形式发布，供学习、二次开发和扩展。

## 演示

<p align="center">
  <img src="docs/images/hardware-eye-mode.jpg" width="58%" alt="硬件 — 眼睛模式">
  <img src="docs/images/hardware-info-mode.jpg" width="26%" alt="硬件 — 信息模式">
</p>

## 核心特性

- 18 种拟态眼睛情绪：Normal、Angry、Glee、Happy、Sad、Worried、Focused、Annoyed、Surprised、Skeptic、Frustrated、Unimpressed、Sleepy、Suspicious、Squint、Furious、Scared、Awe。
- 姿态驱动凝视：MPU6050 的 X/Y 加速度映射为 `[-1, 1]` 区间的眼睛方向。倾斜机身，眼睛跟随。
- 物理扰动 → Scared：总加速度异常时自动触发 *Scared* 情绪，约 1.5 秒后恢复。
- 环境感知：板载 AHT20 每 ~3 秒采集一次温湿度，在设备端、BLE 遥测和 HTTP API 上同时显示。
- 一屏双模：*眼睛模式*（拟态情感眼）和 *信息模式*（时间 / 日期 / 星期 / 温湿度 / WiFi / BLE 状态），通过物理按键一键切换。
- BLE + WiFi 双通道：BLE 作为常开的控制与配网通道；WiFi 提供高速 HTTP 遥测通道。任意一条通道故障，另一条不受影响。
- BLE 辅助 WiFi 配网：首次 WiFi 凭据在 App 内输入后经 BLE 分片下发（含长 SSID/密码的分片处理），设备写入 NVS，每次开机自动重连。
- 本地 HTTP API：`GET /api/status` 与 `GET /api/telemetry` 返回 JSON 状态，可直接对接 LAN / IoT。
- 跨平台移动端：Uni-app / Vue App，提供 BLE 扫描、眼控、X/Y 凝视滑杆、18 情绪选择、WiFi 配网页和深/浅主题。
- 优雅降级：IMU 失败时眼睛/BLE/WiFi 仍正常工作；WiFi 掉线时 BLE 控制与遥测不受影响；BLE 广播丢失时看门狗自动重启广播。

## 系统架构

系统采用五层架构：**App → 通信 → 控制 → 感知 / 表现 → 硬件**，并形成完整的数据闭环：

<p align="center">
  <img src="docs/images/architecture-five-layer.png" width="800" alt="系统五层架构">
</p>

### 核心数据闭环

```
环境 / 姿态变化
   → AHT20 / MPU6050
   → ESP32-C6 数据采集
   → 状态判断与行为逻辑
   → Face 表情 / 凝视引擎
   → LCD 视觉表达
   → BLE / WiFi 遥测
   → 手机 App 状态展示
   → 用户控制指令
   → 回到 ESP32-C6
```

<p align="center">
  <img src="docs/images/data-loop.png" width="800" alt="核心数据闭环">
</p>

## 硬件

当前原型基于 **Waveshare ESP32-C6-LCD-1.47** 平台搭建。

| 模块 | 器件 | 作用 |
| :-- | :-- | :-- |
| 主控 | ESP32-C6 | 系统控制、BLE GATT、WiFi STA、HTTP、NTP、行为引擎 |
| 显示 | ST7789 LCD（172×320，SPI） | 拟态眼睛 + 信息界面 |
| 姿态 | MPU6050（I2C） | 倾斜 → 凝视方向；晃动 → Scared |
| 环境 | AHT20（I2C） | 温度与湿度 |
| 按键 | 轻触按键（接 GPIO 9） | 切换 Eye Mode ↔ Info Mode |
| 电源 | USB / 5V | 系统供电 |

### 引脚定义（ESP32-C6 ↔ LCD / 传感器）

**LCD（SPI）：**

| 功能 | GPIO |
| :-- | :-- |
| MOSI | 6 |
| SCLK | 7 |
| CS | 14 |
| DC | 15 |
| RST | 21 |
| Backlight | 22 |
| SD CS | 4 |

**I2C 总线（MPU6050 + AHT20 共用）：**

| 功能 | GPIO |
| :-- | :-- |
| SDA | 18 |
| SCL | 19 |

**按键：** GPIO 9（内部上拉，下降沿中断，约 300 ms 防抖）

### 硬件一览

<p align="center">
  <img src="docs/images/esp32-c6-lcd.png" width="30%" alt="ESP32-C6 + 1.47 寸 LCD">
  <img src="docs/images/mpu6050.png" width="30%" alt="MPU6050 IMU">
  <img src="docs/images/aht20.png" width="30%" alt="AHT20 温湿度传感器">
</p>

### 推荐 BOM

| 模块 | 器件 | 数量 |
| :-- | :-- | :-- |
| 主控 | ESP32-C6 开发板 | 1 |
| 显示 | ST7789 1.47 寸 LCD（172×320） | 1 |
| 姿态 | MPU6050 模组 | 1 |
| 环境 | AHT20 模组 | 1 |
| 按键 | 6mm 轻触按键 | 1 |
| 电源 | USB-C / 5V 供电 | 1 |
| 外壳 | 3D 打印 / 定制 | 1 |

### 双显示模式设计

- **眼睛模式（默认）** — 核心交互界面。展示拟态眼睛的自动眨眼、姿态跟随凝视、App 凝视控制、随机行为和随机凝视。
- **信息模式** — 约 1 Hz 低频刷新的信息面板，显示当前时间、日期、星期、温湿度和 WiFi / BLE 状态。低频刷新是为了避免不必要的高频全屏重绘。

  移动端 App 同样有对应的 **眼睛模式** 和 **信息模式** 页面（截图见下文"移动端 App"章节）。

设备端用 GPIO 9 上的物理按键切换，App 端用首页"视图模式"卡片切换。

## 固件（ESP32-C6）

固件为单 Arduino 框架工程（`LingMou.ino`），按逻辑分为以下几层：

```
LingMou Firmware
├── Hardware Layer      →  LCD、MPU6050、AHT20、Button
├── Face Engine         →  Blink、Emotion、LookAt、RandomBehavior、RandomLook
├── Communication       →  BLE（NimBLE）、WiFi、HTTP API、NTP
├── Storage             →  Preferences / NVS（WiFi 凭据）
├── UI                  →  Eye Mode、Info Mode
└── Main Scheduler      →  非阻塞 loop()
```

### 启动流程

<p align="center">
  <img src="docs/images/software-flow.png" width="500" alt="固件启动流程">
</p>

BLE 在启动早期就被初始化，保证即使部分外设异常，设备仍可被发现与配网。

### 表情引擎

- 约 30 FPS 动画循环（约 33 ms / 帧），使用 TFT_eSPI **Sprite** 离屏缓冲 + **Arduino_GFX** 输出到 ST7789，避免复杂动画直接刷新整屏造成的闪烁。
- 支持自动眨眼、手动眨眼（`B:1`）、凝视控制（`X:x,y`）、随机凝视、随机行为、情绪切换（`E:n`）、姿态驱动凝视。
- **MPU6050 → 凝视映射：** `ax, ay` 归一化到 `look_x, look_y ∈ [-1, 1]`。机身左/右倾 → 眼睛左/右看；前/后倾 → 眼睛上/下看。这是实体姿态与虚拟表情联动的关键桥梁。
- **晃动 → Scared：** 总加速度 `A = √(ax² + ay² + az²)`；当 `|A − g|` 超过阈值时进入 *Scared* 状态约 1.5 秒，然后恢复 *Normal*。

### 18 种情绪

| 序号 | 情绪 | 序号 | 情绪 | 序号 | 情绪 |
| :- | :-- | :- | :-- | :- | :-- |
| 0 | Normal | 6 | Focused | 12 | Sleepy |
| 1 | Angry | 7 | Annoyed | 13 | Suspicious |
| 2 | Glee | 8 | Surprised | 14 | Squint |
| 3 | Happy | 9 | Skeptic | 15 | Furious |
| 4 | Sad | 10 | Frustrated | 16 | Scared |
| 5 | Worried | 11 | Unimpressed | 17 | Awe |

## 通信

### BLE（NimBLE GATT）

- **设备名：** `LingMou`
- **Service UUID：** `4fafc201-1fb5-459e-8fcc-c5c9c331914b`
- **RX（App → 设备）：** `beb5483e-36e1-4688-b7f5-ea07361b26a8`
- **TX（设备 → App，Notify）：** `beb5483e-36e1-4688-b7f5-ea07361b26a9`

采用轻量文本协议；分片大小根据对端 MTU 自动适配，保证 JSON 状态消息和长 SSID/密码不被截断。

| 指令 | 示例 | 功能 |
| :-- | :-- | :-- |
| 眨眼 | `B:1` | 触发一次眨眼 |
| 随机行为 | `I:1` | 开启随机行为 |
| 随机凝视 | `W:1` | 开启随机凝视 |
| 情绪 | `E:3` | 切换到第 3 号情绪（Happy） |
| 凝视 | `X:0.5,-0.3` | 设置眼睛 X/Y 方向 |
| 获取状态 | `GET_STATUS` | 返回设备状态 |
| 获取遥测 | `GET_TELEMETRY` | 返回环境数据 |
| 手机授时 | `TIME=<epoch>` | 同步系统时间 |
| WiFi SSID | `WIFI_SSID=xxx`（支持 `WIFI_SSID+` 分片） | 设置 WiFi 名称 |
| WiFi 密码 | `WIFI_PASS=xxx`（支持 `WIFI_PASS+`） | 设置密码 |
| 开始联网 | `WIFI_CONNECT` | 保存并连接 |
| 清除网络 | `WIFI_CLEAR` | 清除已保存的 WiFi |

设备内置 **BLE 广播看门狗**：若检测到广播停止会自动重启广播，提高 App 再次连接成功率。

### WiFi 与 HTTP API

WiFi 连接成功后，设备在 80 端口启动本地 WebServer：

```
GET /api/status
GET /api/telemetry
```

**`/api/telemetry` 响应示例：**

```json
{
  "t": 25.6,
  "h": 48.3,
  "aht": 1,
  "time": "14:32:18",
  "wifi": 1
}
```

**`/api/status`** 返回 WiFi 状态、BLE 状态、SSID、IP 和最新遥测数据。

- 单次连接超时约 15 秒，失败后约 30 秒重试，启用自动重连。
- WiFi 凭据保存到 NVS，每次开机自动连接已保存的网络。
- WiFi 不可达时，**BLE 仍可完全正常工作**，负责控制 + 遥测兜底。

### 时间同步

支持两种授时方式，保证时钟始终正确：

- **BLE 手机授时**（`TIME=<unix>`）：无 WiFi 时使用。
- **NTP**（`pool.ntp.org`，UTC+8）：WiFi 已连接时使用。

## 移动端 App（Uni-app / Vue）

配套的 *灵眸控制中心* App 基于 **Uni-app / Vue** 开发，主要能力：

- 跨 Android 版本的 BLE 权限处理（Android 12+ 的附近设备权限 vs. 旧版的定位权限）。
- BLE 扫描：优先按 Service UUID 过滤，失败时回退到按设备名 / 历史 ID 识别。
- 设备发现、GATT 连接、特征订阅、指令发送。
- **Bento Grid + Glass Card** 风格首页：链路状态、视图模式、眼控、18 情绪选择、X/Y 凝视滑杆、环境数据实时面板。
- WiFi 配网页：BLE → 配网 → 在线 三阶段进度展示，并显示设备 IP。
- 支持浅色 / 深色主题。

#### 眼睛模式（Eye Mode）

<p align="center">
  <img src="docs/images/app-eye-day.jpg" width="30%" alt="App — 眼睛模式（白天）">
  &nbsp;
  <img src="docs/images/app-eye-night.jpg" width="30%" alt="App — 眼睛模式（黑夜）">
</p>
<p align="center"><i>眼睛模式：18 情绪选择 + 眼控。左：白天，右：黑夜。</i></p>

#### 信息模式（Info Mode）

<p align="center">
  <img src="docs/images/app-info-day.jpg" width="30%" alt="App — 信息模式（白天）">
  &nbsp;
  <img src="docs/images/app-info-night.jpg" width="30%" alt="App — 信息模式（黑夜）">
</p>
<p align="center"><i>信息模式：实时时钟 + 温湿度（含舒适度指示）+ AHT20 / WiFi 实时状态。左：白天，右：黑夜。</i></p>

#### WiFi 配网

<p align="center">
  <img src="docs/images/wifi-setup.jpg" width="30%" alt="App — WiFi 配网">
</p>
<p align="center"><i>WiFi 配网：BLE → 配网 → 在线 三阶段进度，并显示设备 IP。</i></p>

## 可靠性

| 故障 | 行为 |
| :-- | :-- |
| MPU6050 初始化失败 | 眼睛、BLE、WiFi、AHT20 仍工作，仅姿态驱动凝视关闭。 |
| AHT20 初始化失败 | 眼睛、姿态、BLE、WiFi 仍工作，温湿度显示为"不可用"。 |
| WiFi 掉线 | HTTP 遥测不可用；BLE 控制与遥测继续工作。 |
| BLE 广播丢失 | 看门狗约 2 秒内重启广播。 |
| 眼睛动画 | 采用 Sprite 离屏渲染，避免整屏闪烁。 |
| 信息模式 | 约 1 Hz 低频刷新，避免不必要的高频重绘。 |

## 快速上手

### 1. 硬件

按上面的引脚表把模块接到 ESP32-C6（或者直接用 Waveshare ESP32-C6-LCD-1.47 平台，pin 全部一致），USB-C 供电。

### 2. 固件烧录

固件位于 `Firmware/LingMou/LingMou.ino`，目标框架为 **ESP32-C6 + Arduino**。

**Arduino IDE / Arduino CLI：**

1. 通过 Board Manager 安装 **ESP32** 板支持包（≥ 3.x）。
2. 安装以下库：
   - `Arduino_GFX_Library`
   - `TFT_eSPI`
   - `NimBLE-Arduino`
   - `Adafruit AHT20`
   - `Adafruit MPU6050`（外加 `Adafruit Unified Sensor`、`Adafruit BusIO`）
3. 打开 `Firmware/LingMou/LingMou.ino`。
4. 选择开发板：`ESP32C6 Dev Module`（如果安装了 Waveshare 的板支持包，可选对应型号）。
5. 编译并烧录。

> 所有引脚定义都在 `LingMou.ino` 文件顶部，自制板时按需修改。

### 3. 移动端 App

App 源码位于 `App/`（Uni-app 工程，入口为 `index.vue`）。

1. 在 **HBuilderX** 中打开 `App/`。
2. 运行到手机（Android / iOS）或内置浏览器。
3. 授予蓝牙权限（Android 12+ 还需要附近设备权限）。
4. App 自动扫描名为 `LingMou` 的设备，找到后点击连接。

### 4. 首次使用

1. 设备上电，默认进入 *眼睛模式*（Normal 情绪）。
2. 打开 App，扫描并通过 BLE 连接设备。
3. 打开 **WiFi 配网页**，输入 SSID 和密码，点击"保存并连接"。
4. 设备加入 WiFi 后会上报 IP——此时也能通过 HTTP API 访问。
5. 试试倾斜机身（眼睛跟随）和轻轻摇晃一下（触发 *Scared*）。
6. 按一下物理按键切换到 *信息模式*，可看到时间 / 温湿度 / WiFi / BLE 状态。

## 仓库结构

```
LingMou_SmallTeamProject/
├── Firmware/                       ESP32-C6 Arduino 工程
│   └── LingMou/
│       ├── LingMou.ino
│       ├── LingMouEngine.cpp
│       └── LingMouEngine.h
├── App/                            Uni-app / Vue 移动端
│   ├── App.vue
│   ├── index.html
│   ├── main.js
│   ├── manifest.json
│   ├── pages.json
│   ├── pages/index/index.vue
│   └── static/logo.png
├── docs/
│   ├── images/                     README / 文档配图
│   └── LingMou_Project_Design.md   完整设计文档
├── .gitignore
├── LICENSE
├── README.md                       英文 README（默认）
└── README.zh-CN.md                 简体中文 README
```

当前固件采用紧凑结构——主程序 `LingMou.ino` 加上独立的 `LingMouEngine` 模块，后续会进一步按 `display/`、`sensors/`、`communication/`、`behavior/`、`storage/` 模块化重构。

## 测试

项目配套 20 项功能测试与 8 项验收指标：

| 指标 | 目标 |
| :-- | :-- |
| 眼睛动画 | ≥ 25 FPS |
| BLE 控制响应 | < 300 ms |
| App → 设备情绪切换 | < 500 ms |
| BLE 重连 | 自动恢复广播 |
| 温湿度刷新 | ≤ 3 秒 |
| WiFi 配网成功率 | ≥ 95%（正常网络环境） |
| 连续运行 | ≥ 24 小时无死机 |
| WiFi 故障 | 不影响 BLE 核心控制 |

## 路线图

当前原型在功能上已经较完整。后续演进规划：

- **第一阶段 — 完善现有系统：** 统一 BLE 指令协议、增加设备状态机、App 自动重连、更多遥测、动画效率优化、参数配置页、24/48 小时稳定性测试。
- **第二阶段 — 加入声音：** MEMS 麦克风 + I2S 功放 + 扬声器、提示音、语音唤醒、本地关键词识别。系统升级为视觉 + 姿态 + 环境 + **声音** 多模态交互设备。
- **第三阶段 — AI 能力：** 调用云端 / 本地大模型，返回结构化动作（如 `{emotion, look_x, look_y, action}`），设备负责执行——*"云端智能 + 本地实时执行"*。
- **第四阶段 — 自主行为：** 引入内部状态（`Energy, Mood, Curiosity, Comfort, Attention, Sleepiness`），由环境和用户行为驱动演化，进而决定表情和动作（长时间空闲 → *Sleepy*；被拿起 → *Awe / Happy*）。
- **工程方向：** WiFi OTA、统一 JSON 协议、正式状态机、低功耗模式、参数持久化（亮度、默认模式、灵敏度、设备名）。

## 贡献

欢迎提 Issue 和 PR，特别欢迎以下方向：

- 新的情绪 / 更好的眼睛渲染
- 更多传感器（光、声、姿态）
- 通过 HTTP API 对接 Home Assistant / MQTT
- 移植到其他 ESP32 + LCD 组合

提交前请跑一遍冒烟测试（上电 → BLE 连接 → 倾斜 → 晃动），确保基本闭环正常。

## 许可证

本项目基于 **MIT 协议**开源，详见 [`LICENSE`](LICENSE)。如需更换为其他 OSI 协议（Apache-2.0 等）也可以直接替换该文件。

## 致谢

- **Waveshare** — 提供 ESP32-C6-LCD-1.47 开发平台。
- **Arduino_GFX**、**TFT_eSPI**、**NimBLE-Arduino**、**Adafruit AHT20 / MPU6050** — 优秀的开源库。
- **Uni-app / Vue** — 跨平台移动端开发框架。

---

> *灵眸 LingMou — 让机器拥有会回应世界的眼睛。*
> *Give machines eyes that respond to the world.*
