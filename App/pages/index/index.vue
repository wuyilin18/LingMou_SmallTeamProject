<template>
  <view :class="['app-wrapper', isDarkMode ? 'dark-theme' : 'light-theme']">
    
    <view class="ambient-orb orb-top"></view>
    <view class="ambient-orb orb-bottom"></view>

    <view class="splash-screen" v-if="showSplash">
      <view class="splash-content">
        <view class="aura-glow"></view>
        <view class="eye-composition-root">
          <view class="eye-corner-tail tail-left"></view>
          <view class="eye-corner-tail tail-right"></view>
          <view class="pear-shape-outline"></view>
          <view class="pear-shape-mask">
            <view class="glass-pupil">
              <view class="pupil-highlight"></view>
            </view>
          </view>
        </view>
        <view class="zen-text-group">
          <text class="splash-title clean-text">灵 眸</text>
        </view>
      </view>
    </view>

    <view class="container z-content">
      
      <view class="header">
        <view class="title-group">
          <text class="title gradient-text">灵眸</text>
          <text class="subtitle">掌心里的硅基生命控制中枢。</text>
        </view>
        <view class="theme-toggle-complex" @click="toggleTheme">
          <view class="theme-scene" :class="isDarkMode ? 'scene-night' : 'scene-day'">
            <view class="day-bg"><view class="sun"></view><view class="cloud cloud-1"></view><view class="cloud cloud-2"></view></view>
            <view class="night-bg"><view class="moon"></view><view class="star star-1"></view><view class="star star-2"></view><view class="star star-3"></view></view>
          </view>
        </view>
      </view>

      <view class="bento-grid">
        
	<view class="bento-item glass-card status-card-wide">
          <view class="card-top-bar">
            <view class="status-header">
              <view :class="['status-dot', isConnected ? 'dot-online' : 'dot-offline']"></view>
              <text class="card-title" style="margin-bottom: 0;">BLE Link Status</text>
            </view>
            <view class="battery-indicator" v-if="isConnected">
              <text class="battery-icon">🔋</text>
              <text class="battery-text">{{ batteryLevel }}%</text>
            </view>
          </view>
          
          <view class="card-bottom-bar">
            <view class="status-main">
              <text class="main-title">LingMou Core</text>
              <text class="sub-title">
                {{ statusText }}
              </text>
            </view>
            <button :class="['ble-scan-btn', isScanning ? 'btn-scanning' : '']" @click="toggleBleConnection">
              <text class="btn-text">{{ isConnected ? '断开蓝牙' : (isScanning ? '寻呼中...' : '寻呼设备') }}</text>
            </button>
          </view>
        </view>
        <view class="bento-item glass-card override-card">
          <button class="action-icon-btn blink-btn" @click="sendBlink">
            <view class="icon-wrapper"><text class="icon">✨</text></view>
            <text class="btn-label">灵瞬</text>
          </button>
          
          <button :class="['action-icon-btn', 'auto-btn', isIllusionOn ? 'btn-active' : '']" @click="toggleIllusion">
            <view class="icon-wrapper"><text class="icon">🎭</text></view>
            <text class="btn-label">幻相</text>
          </button>
          
          <button :class="['action-icon-btn', 'manual-btn', isWanderOn ? 'btn-active' : '']" @click="toggleWander">
            <view class="icon-wrapper"><text class="icon">🛸</text></view>
            <text class="btn-label">游荡</text>
          </button>
        </view>

        <view class="bento-item glass-card tracking-card">
          <view class="card-title" style="margin-bottom: 12px;">Eye Tracking</view>
          <view class="slider-row">
            <text class="axis">X</text>
            <slider class="cyber-slider" :value="lookX" min="-1" max="1" step="0.1" @change="onLookXChange" :activeColor="isDarkMode ? '#22d3ee' : '#0891b2'" block-size="16" />
          </view>
          <view class="slider-row">
            <text class="axis">Y</text>
            <slider class="cyber-slider" :value="lookY" min="-1" max="1" step="0.1" @change="onLookYChange" :activeColor="isDarkMode ? '#22d3ee' : '#0891b2'" block-size="16" />
          </view>
        </view>

        <view class="bento-item glass-card emotion-card">
          <view class="card-title">Emotions</view>
          <view class="chip-grid">
            <button class="chip-btn" v-for="emo in emotions" :key="emo" @click="sendEmotion(emo)">
              {{ emo }}
            </button>
          </view>
        </view>

      </view> 
    </view>
  </view>
</template>

<script>
export default {
  data() {
    return {
      showSplash: true, 
      isDarkMode: true, 
      
      // 📡 BLE 蓝牙底层变量
      isConnected: false,
      isScanning: false,
      statusText: '蓝牙已断开',
      deviceId: '',
      serviceId: '4fafc201-1fb5-459e-8fcc-c5c9c331914b', // ESP32 默认指令服务端 UUID
      charId: 'beb5483e-36e1-4688-b7f5-ea07361b26a8',    // ESP32 默认指令写入特征值 UUID
      targetDeviceName: 'LingMou', // 锁定寻找名为 LingMou 的设备
      
      // 🔋 新增：真实的电池生命体征变量和标准 UUID
      batteryLevel: '--', 
      batteryServiceId: '0000180F-0000-1000-8000-00805F9B34FB', // BLE 国际标准电池服务
      batteryCharId: '00002A19-0000-1000-8000-00805F9B34FB',    // BLE 国际标准电池特征值

      // 物理外挂状态
      isIllusionOn: false,
      isWanderOn: false,

      lookX: 0,
      lookY: 0,
      emotions: [
        "Normal","Angry","Glee","Happy","Sad","Worried",
        "Focused","Annoyed","Surprised","Skeptic","Frustrated",
        "Unimpressed","Sleepy","Suspicious","Squint","Furious","Scared","Awe"
      ]
    };
  },
  onLoad() {
    setTimeout(() => { this.showSplash = false; }, 4000); 
    this.syncSystemTheme(); 
    // 自动初始化蓝牙模块
    this.initBluetooth();
  },
  onUnload() {
    this.closeBluetooth();
  },
  methods: {
    syncSystemTheme() {
      const sysInfo = uni.getSystemInfoSync();
      if (sysInfo.theme) {
        this.isDarkMode = sysInfo.theme === 'dark';
      } else {
        const currentHour = new Date().getHours();
        this.isDarkMode = (currentHour < 6 || currentHour >= 18);
      }
      this.updateNavColor();
      if (uni.onThemeChange) {
        uni.onThemeChange((res) => {
          this.isDarkMode = res.theme === 'dark';
          this.updateNavColor();
        });
      }
    },
    toggleTheme() {
      this.isDarkMode = !this.isDarkMode;
      this.updateNavColor();
    },
    updateNavColor() {
      uni.setNavigationBarColor({
        frontColor: this.isDarkMode ? '#ffffff' : '#000000',
        backgroundColor: this.isDarkMode ? '#0f172a' : '#f8fafc' 
      });
    },

    // ==========================================
    // 📡 纯血 BLE 蓝牙控制中枢引擎
    // ==========================================
    initBluetooth() {
      // #ifdef H5
      uni.showToast({ title: '网页端不支持蓝牙，请在真机或小程序运行！', icon: 'none' });
      return;
      // #endif

      uni.openBluetoothAdapter({
        success: (res) => {
          console.log('蓝牙模块初始化成功');
        },
        fail: (err) => {
          uni.showToast({ title: '请先打开手机蓝牙！', icon: 'none' });
          this.statusText = '手机蓝牙未开启';
        }
      });
    },

    toggleBleConnection() {
      if (this.isConnected) {
        this.disconnectBle();
      } else {
        if (this.isScanning) {
          this.stopScan();
        } else {
          this.scanForLingMou();
        }
      }
    },

    scanForLingMou() {
      this.isScanning = true;
      this.statusText = '正在扫描周围空气中的灵眸信号...';
      
      uni.startBluetoothDevicesDiscovery({
        allowDuplicatesKey: false,
        success: (res) => {
          // 监听寻呼结果
          uni.onBluetoothDeviceFound((devices) => {
            let deviceList = devices.devices;
            for (let i = 0; i < deviceList.length; i++) {
              if (deviceList[i].name === this.targetDeviceName || deviceList[i].localName === this.targetDeviceName) {
                console.log('发现灵眸本体：', deviceList[i]);
                this.deviceId = deviceList[i].deviceId;
                this.stopScan();
                this.connectBle();
                break;
              }
            }
          });
        },
        fail: (err) => {
          this.isScanning = false;
          uni.showToast({ title: '扫描启动失败', icon: 'none' });
        }
      });

      // 10秒扫描超时保护
      setTimeout(() => {
        if (this.isScanning) {
          this.stopScan();
          this.statusText = '周围未发现灵眸信号';
          uni.showToast({ title: '扫描超时', icon: 'none' });
        }
      }, 10000);
    },

    stopScan() {
      uni.stopBluetoothDevicesDiscovery();
      this.isScanning = false;
    },

    connectBle() {
      this.statusText = '正在建立蓝牙连接...';
      uni.showLoading({ title: '连接中' });
      
      uni.createBLEConnection({
        deviceId: this.deviceId,
        timeout: 5000,
        success: (res) => {
          uni.hideLoading();
          this.isConnected = true;
          this.statusText = '物理直连已就绪';
          uni.showToast({ title: '蓝牙已接管！', icon: 'success' });
          
          // 🔴 1. 监听意外断开
          uni.onBLEConnectionStateChange((stateRes) => {
            if (!stateRes.connected) {
              this.isConnected = false;
              this.statusText = '蓝牙意外断开';
              this.batteryLevel = '--'; // 断线时立刻清空电量
              uni.showToast({ title: '连接已断开', icon: 'none' });
            }
          });

          // 🔴 2. 开启全时监听：接收单片机发来的任何数据（包括电量）
          uni.onBLECharacteristicValueChange((characteristic) => {
            // 匹配电池服务特征值 (2A19)
            if (characteristic.characteristicId.toUpperCase().includes('2A19')) {
              let view = new DataView(characteristic.value);
              this.batteryLevel = view.getUint8(0);
              console.log('⚡ 收到灵眸实时电量:', this.batteryLevel + '%');
            }
          });

		// 🔴 3. 动态跨平台蓝牙订阅流程 (修复 iOS/Android UUID 差异)
          setTimeout(() => {
            uni.getBLEDeviceServices({
              deviceId: this.deviceId,
              success: (res) => {
                // 动态匹配系统分配的真实电池 Service UUID
                let realBatService = res.services.find(s => s.uuid.toUpperCase().includes('180F'));
                if (!realBatService) return;

                uni.getBLEDeviceCharacteristics({
                  deviceId: this.deviceId,
                  serviceId: realBatService.uuid,
                  success: (cRes) => {
                    // 动态匹配真实电池特征值 UUID
                    let realBatChar = cRes.characteristics.find(c => c.uuid.toUpperCase().includes('2A19'));
                    if (!realBatChar) return;

                    // 开启电池电量变动通知订阅
                    uni.notifyBLECharacteristicValueChange({
                      deviceId: this.deviceId,
                      serviceId: realBatService.uuid,
                      characteristicId: realBatChar.uuid,
                      state: true, 
                      success: () => {
                        // 订阅成功后，主动读取一次初始电量
                        uni.readBLECharacteristicValue({
                          deviceId: this.deviceId,
                          serviceId: realBatService.uuid,
                          characteristicId: realBatChar.uuid
                        });
                      }
                    });
                  }
                });
              }
            });
          }, 1500);
        },
        fail: (err) => {
          uni.hideLoading();
          this.statusText = '连接失败，请靠近设备';
          uni.showToast({ title: '连接失败', icon: 'none' });
        }
      });
    },

    disconnectBle() {
      if (this.deviceId) {
        uni.closeBLEConnection({
          deviceId: this.deviceId,
          success: () => {
            this.isConnected = false;
            this.statusText = '蓝牙已断开';
            this.batteryLevel = '--';
          }
        });
      }
    },

    closeBluetooth() {
      this.disconnectBle();
      uni.closeBluetoothAdapter();
    },

    // ==========================================
    // 🧬 极简 MTU 发射协议 (短包高速传输)
    // ==========================================
	sendBLECommand(cmdString) {
      if (!this.isConnected) {
        uni.showToast({ title: '蓝牙未连接', icon: 'none' });
        return;
      }
      
      let buffer = new ArrayBuffer(cmdString.length);
      let dataView = new Uint8Array(buffer);
      for (let i = 0; i < cmdString.length; i++) {
        dataView[i] = cmdString.charCodeAt(i);
      }

      // 🔴 使用你原来定义的 UUID 变量即可，因为这是我们自定义的 128 位 UUID，不会有平台差异
      uni.writeBLECharacteristicValue({
        deviceId: this.deviceId,
        serviceId: this.serviceId,
        characteristicId: this.charId,
        value: buffer,
        writeType: 'writeNoResponse', // 🚀 极速盲发模式，极大降低通信延迟
        success: () => { console.log('指令击中:', cmdString); },
        fail: (err) => { console.error('指令发射失败:', err); }
      });
    },
    
    sendBlink() { 
      this.sendBLECommand('B:1'); 
    },
    
    toggleIllusion() { 
      this.isIllusionOn = !this.isIllusionOn; 
      this.sendBLECommand(`I:${this.isIllusionOn ? 1 : 0}`); 
    },
    
    toggleWander() { 
      this.isWanderOn = !this.isWanderOn; 
      this.sendBLECommand(`W:${this.isWanderOn ? 1 : 0}`); 
    },

    sendEmotion(emo) { 
      this.isIllusionOn = false;
      let emoIndex = this.emotions.indexOf(emo);
      this.sendBLECommand(`E:${emoIndex}`); 
    },
    
    onLookXChange(e) {
      this.isWanderOn = false;
      this.lookX = e.detail.value;
      this.sendBLECommand(`X:${this.lookX},${this.lookY}`);
    },
    
    onLookYChange(e) {
      this.isWanderOn = false;
      this.lookY = e.detail.value;
      this.sendBLECommand(`X:${this.lookX},${this.lookY}`);
    }
  }
};
</script>

<style>
/* =========================================
   🎨 全局变量 
   ========================================= */
.light-theme {
  --bg-color: #f8fafc;          
  --card-bg: rgba(255, 255, 255, 0.7); 
  --text-main: #1e293b;         
  --text-sub: #64748b;          
  --border-color: rgba(226, 232, 240, 0.8);
  --btn-bg: #f1f5f9;            
  --btn-hover: #e2e8f0;         
  --accent-color: #0891b2;      
  --gradient-start: #1e293b;    
  --gradient-end: #0891b2;      
  --shadow-color: rgba(15, 23, 42, 0.05);
  --orb-color: rgba(8, 145, 178, 0.15);
}

.dark-theme {
  --bg-color: #0f172a;          
  --card-bg: rgba(30, 41, 59, 0.6); 
  --text-main: #f8fafc;         
  --text-sub: #94a3b8;          
  --border-color: rgba(51, 65, 85, 0.6);      
  --btn-bg: rgba(51, 65, 85, 0.5);            
  --btn-hover: rgba(71, 85, 105, 0.8);         
  --accent-color: #22d3ee;      
  --gradient-start: #cbd5e1;    
  --gradient-end: #22d3ee;      
  --shadow-color: rgba(0, 0, 0, 0.3);
  --orb-color: rgba(34, 211, 238, 0.08);
}

/* =========================================
   🌌 环境氛围与底层结构
   ========================================= */
page { background-color: transparent; height: 100%; }
.app-wrapper { background-color: var(--bg-color); color: var(--text-main); min-height: 100vh; position: relative; overflow-x: hidden; transition: background-color 0.4s ease, color 0.4s ease; }
.ambient-orb { position: absolute; border-radius: 50%; background: var(--orb-color); filter: blur(60px); z-index: 0; pointer-events: none; }
.orb-top { width: 300px; height: 300px; top: -100px; right: -50px; }
.orb-bottom { width: 250px; height: 250px; bottom: 10%; left: -80px; }
.z-content { position: relative; z-index: 10; }
.container { padding: 50px 20px 60px; font-family: -apple-system, BlinkMacSystemFont, sans-serif; max-width: 600px; margin: 0 auto; }

/* =========================================
   🌌 开屏动画 (保留你的绝美设计)
   ========================================= */
.splash-screen { position: fixed; top: 0; left: 0; width: 100vw; height: 100vh; background-color: #050b14; z-index: 99999; display: flex; justify-content: center; align-items: center; animation: splash-blur-out 0.8s cubic-bezier(0.4, 0, 0.2, 1) 3.2s forwards; }
.splash-content { display: flex; flex-direction: column; align-items: center; gap: 8px; position: relative; }
.aura-glow { position: absolute; top: 40%; left: 50%; transform: translate(-50%, -50%); width: 240px; height: 80px; background: radial-gradient(ellipse, rgba(6, 182, 212, 0.3), transparent 70%); filter: blur(20px); animation: aura-pulse 3s ease-in-out infinite alternate; }
.eye-composition-root { position: relative; width: 280px; height: 160px; display: flex; justify-content: center; align-items: center; }
.pear-shape-outline, .pear-shape-mask { position: absolute; width: 155px; height: 155px; border-radius: 90% 10%; transform-style: preserve-3d; -webkit-transform-style: preserve-3d; backface-visibility: hidden; -webkit-backface-visibility: hidden; transform: scaleY(0.01) rotate(45deg) translateZ(0); }
.pear-shape-outline { background-color: #020612; border: 3px solid rgba(165, 243, 252, 0.9); box-shadow: 0 0 25px rgba(6, 182, 212, 0.6), inset 0 0 35px rgba(8, 145, 178, 0.9); animation: pear-open-outline 1.4s cubic-bezier(0.68, -0.15, 0.265, 1.35) 0.3s forwards; z-index: 1; }
.pear-shape-mask { overflow: hidden; background: transparent; display: flex; justify-content: center; align-items: center; animation: pear-open-mask 1.4s cubic-bezier(0.68, -0.15, 0.265, 1.35) 0.3s forwards; z-index: 5; }
.glass-pupil { transform: rotate(-45deg) scaleY(1.282) translateZ(0); width: 76px; height: 76px; border-radius: 50%; background: radial-gradient(circle at 50% 20%, #0f172a 10%, #0369a1 50%, #06b6d4 80%, #cffafe 100%); box-shadow: inset 0 -10px 20px rgba(165, 243, 252, 0.9), inset 0 10px 15px rgba(0,0,0,0.8), 0 0 25px rgba(34, 211, 238, 0.6); position: relative; opacity: 0; animation: pupil-fade-in 1.2s ease-out 0.8s forwards; }
.pupil-highlight { position: absolute; top: 12%; left: 20%; width: 18px; height: 10px; border-radius: 50%; background: rgba(255, 255, 255, 0.9); transform: rotate(-25deg); filter: blur(0.5px); }
.eye-corner-tail { position: absolute; top: 50%; width: 70px; height: 3px; transform: translateY(-50%); z-index: 0; opacity: 0; animation: tails-extend 1.2s ease-out 0.6s forwards; }
.tail-left { left: 0; background: linear-gradient(270deg, rgba(165,243,252,0.8), transparent); box-shadow: -5px 0 10px rgba(6,182,212,0.5); }
.tail-right { right: 0; background: linear-gradient(90deg, rgba(165,243,252,0.8), transparent); box-shadow: 5px 0 10px rgba(6,182,212,0.5); }
.zen-text-group { display: flex; justify-content: center; align-items: center; margin-top: 0px; z-index: 10; }
.clean-text { font-family: -apple-system, "PingFang SC", "Helvetica Neue", sans-serif; font-size: 28px; font-weight: 300; letter-spacing: 16px; margin-right: -16px; color: rgba(165, 243, 252, 0.95); text-shadow: 0 0 12px rgba(165, 243, 252, 0.3); opacity: 0; animation: text-sync-fade 1.4s cubic-bezier(0.2, 0.8, 0.2, 1) 0.3s forwards; }
@keyframes text-sync-fade { 0% { opacity: 0; transform: translateY(8px); } 100% { opacity: 1; transform: translateY(0); } }
@keyframes pear-open-outline { 0% { transform: scaleY(0.01) rotate(45deg) translateZ(0); box-shadow: 0 0 5px rgba(6,182,212,0.2); } 40% { transform: scaleY(0.25) rotate(45deg) translateZ(0); box-shadow: 0 0 15px rgba(6,182,212,0.5); } 100% { transform: scaleY(0.78) rotate(45deg) translateZ(0); box-shadow: 0 0 45px rgba(6,182,212,0.8), inset 0 0 40px rgba(8,145,178,0.9); } }
@keyframes pear-open-mask { 0% { transform: scaleY(0.01) rotate(45deg) translateZ(0); } 40% { transform: scaleY(0.25) rotate(45deg) translateZ(0); } 100% { transform: scaleY(0.78) rotate(45deg) translateZ(0); } }
@keyframes pupil-fade-in { 0% { opacity: 0; } 100% { opacity: 1; } }
@keyframes tails-extend { 0% { width: 10px; opacity: 0; } 100% { width: 70px; opacity: 1; } }
@keyframes aura-pulse { 0% { opacity: 0.6; transform: translate(-50%, -50%) scale(0.9); } 100% { opacity: 1; transform: translate(-50%, -50%) scale(1.1); } }
@keyframes text-fade-in { to { opacity: 0.9; transform: translateY(-5px); } }
@keyframes splash-blur-out { 0% { opacity: 1; filter: blur(0px); transform: scale(1); } 100% { opacity: 0; visibility: hidden; filter: blur(30px); transform: scale(1.3); } }

/* =========================================
   ✨ Header 
   ========================================= */
.header { display: flex; justify-content: space-between; align-items: flex-start; margin-bottom: 25px; }
.title-group { display: flex; flex-direction: column; margin-right: 20px; }
.gradient-text { font-size: 34px; font-weight: 700; background: linear-gradient(to right, var(--gradient-start), var(--gradient-end)); -webkit-background-clip: text; color: transparent; margin-bottom: 4px; letter-spacing: 1px; }
.subtitle { font-size: 13px; color: var(--text-sub); }

.theme-toggle-complex { width: 44px; height: 44px; border-radius: 50%; overflow: hidden; cursor: pointer; box-shadow: 0 4px 15px var(--shadow-color); border: 1px solid var(--border-color); position: relative; flex-shrink: 0; }
.theme-scene { width: 100%; height: 100%; position: relative; }
.day-bg, .night-bg { position: absolute; inset: 0; width: 100%; height: 100%; transition: opacity 0.5s ease, transform 0.5s ease; }
.day-bg { background: linear-gradient(to bottom, #22D3EE, #7FD3F2); opacity: 0; transform: translateY(20px); }
.scene-day .day-bg { opacity: 1; transform: translateY(0); }
.sun { position: absolute; top: 14px; left: 50%; transform: translateX(-50%); width: 16px; height: 16px; background-color: #fde047; border-radius: 50%; box-shadow: 0 0 10px rgba(255,204,0,0.8); }
.cloud { position: absolute; background: white; border-radius: 10px; }
.cloud-1 { width: 12px; height: 5px; left: 6px; top: 22px; animation: cloud-move-1 4s infinite alternate ease-in-out; }
.cloud-2 { width: 8px; height: 4px; right: 6px; top: 10px; animation: cloud-move-2 5s infinite alternate ease-in-out; }
.night-bg { background: linear-gradient(to bottom, #0f1729, #2c3e50); opacity: 0; transform: translateY(-20px); }
.scene-night .night-bg { opacity: 1; transform: translateY(0); }
.moon { position: absolute; top: 12px; left: 14px; width: 14px; height: 14px; background-color: #f3f4f6; border-radius: 50%; box-shadow: 0 0 5px rgba(255,255,255,0.5); }
.star { position: absolute; background: white; border-radius: 50%; }
.star-1 { width: 2px; height: 2px; top: 8px; right: 12px; animation: twinkle 2s infinite alternate; }
.star-2 { width: 2px; height: 2px; top: 18px; right: 8px; animation: twinkle 3s infinite alternate 0.5s; }
.star-3 { width: 2px; height: 2px; top: 14px; left: 8px; animation: twinkle 2.5s infinite alternate 1s; }
@keyframes cloud-move-1 { 0% { transform: translateX(0); } 100% { transform: translateX(4px); } }
@keyframes cloud-move-2 { 0% { transform: translateX(0); } 100% { transform: translateX(-4px); } }
@keyframes twinkle { 0% { opacity: 0.2; transform: scale(0.8); } 100% { opacity: 1; transform: scale(1.2); } }

/* =========================================
   🧩 进化版网格布局 
   ========================================= */
.bento-grid { display: grid; grid-template-columns: repeat(2, 1fr); gap: 16px; }

.bento-item { background-color: var(--card-bg); backdrop-filter: blur(16px); -webkit-backdrop-filter: blur(16px); box-shadow: 0 8px 32px var(--shadow-color); border: 1px solid var(--border-color); padding: 20px; transition: transform 0.3s ease; }

/* 1. 顶层状态舱  */
.status-card-wide { 
  grid-column: span 2; 
  border-radius: 28px; 
  display: flex; 
  flex-direction: column; 
  gap: 18px; 
  padding: 24px 24px;
}
.card-top-bar { display: flex; justify-content: space-between; align-items: center; width: 100%; }
.card-bottom-bar { display: flex; justify-content: space-between; align-items: flex-end; width: 100%; }
.status-header { display: flex; align-items: center; gap: 8px; margin-bottom: 0; }
.status-dot { width: 10px; height: 10px; border-radius: 50%; transition: all 0.3s; flex-shrink: 0;}
.status-left, .status-right { display: none; /* 彻底封杀旧版占位 */ }

/* 2. 快捷指令舱 (灵瞬、幻相、游荡) - 修复横向居中排列 */
.override-card { 
  grid-column: span 2; 
  border-radius: 20px; 
  padding: 24px 16px; 
  display: flex; 
  flex-direction: row; 
  justify-content: space-evenly; 
  align-items: center; 
}

/* 3. 摇杆追踪舱 - 修复宽度 */
.tracking-card { grid-column: span 2; border-radius: 10px 32px 10px 32px; }

/* 4. 情绪芯片舱 - 修复宽度 */
.emotion-card { grid-column: span 2; border-radius: 24px; }

.card-title { font-size: 14px; font-weight: 600; color: var(--text-main); margin-bottom: 16px; }

/* =========================================
   🎛️ 蓝牙组件 UI
   ========================================= */
.status-header { display: flex; align-items: center; gap: 10px; margin-bottom: 15px; }
.status-dot { width: 10px; height: 10px; border-radius: 50%; transition: all 0.3s;}
.dot-online { background-color: #1ED760; box-shadow: 0 0 10px rgba(30, 215, 96, 0.6); }
.dot-offline { background-color: #FF4D4D; box-shadow: 0 0 10px rgba(255, 77, 77, 0.6); }
.main-title { font-size: 22px; font-weight: 700; display: block; margin-bottom: 4px; }
.sub-title { font-size: 12px; color: var(--text-sub); }

.battery-indicator { display: flex; align-items: center; gap: 4px; background: rgba(34, 211, 238, 0.1); padding: 4px 10px; border-radius: 20px; border: 1px solid rgba(34, 211, 238, 0.3); }
.battery-icon { font-size: 14px; }
.battery-text { font-size: 12px; font-weight: 700; color: var(--accent-color); }

.ble-scan-btn { 
  background: linear-gradient(135deg, var(--gradient-start), var(--gradient-end)); 
  border: none; border-radius: 20px; padding: 0 20px; height: 38px; display: flex; align-items: center; justify-content: center; box-shadow: 0 4px 15px rgba(8, 145, 178, 0.3); transition: all 0.3s; margin: 0;
}
.ble-scan-btn::after { display: none; }
.ble-scan-btn:active { transform: scale(0.95); }
.btn-scanning { animation: pulse-btn 1.5s infinite alternate; background: var(--btn-bg); border: 1px solid var(--border-color); box-shadow: none;}
.btn-scanning .btn-text { color: var(--text-main); }
.btn-text { font-size: 13px; font-weight: 600; color: white; }

@keyframes pulse-btn { 0% { opacity: 1; } 100% { opacity: 0.6; } }

/* =========================================
   ✨ 控制按键
   ========================================= */
.action-icon-btn { display: flex; flex-direction: column; align-items: center; justify-content: center; background: transparent; border: none; gap: 10px; padding: 0; overflow: visible;}
.action-icon-btn::after { display: none; }
.action-icon-btn .icon-wrapper { width: 56px; height: 56px; border-radius: 18px; background: var(--btn-bg); border: 1px solid var(--border-color); display: flex; align-items: center; justify-content: center; font-size: 26px; box-shadow: 0 8px 16px var(--shadow-color), inset 0 2px 4px rgba(255,255,255,0.05); transition: all 0.3s cubic-bezier(0.25, 0.8, 0.25, 1); }
.action-icon-btn .btn-label { font-size: 13px; color: var(--text-main); font-weight: 500; letter-spacing: 2px; transition: color 0.3s; }
.action-icon-btn:active .icon-wrapper { transform: translateY(4px) scale(0.9); background: rgba(34, 211, 238, 0.15); border-color: rgba(34, 211, 238, 0.6); box-shadow: 0 0 20px rgba(34, 211, 238, 0.4), inset 0 0 12px rgba(34, 211, 238, 0.3); }
.action-icon-btn:active .btn-label { color: var(--accent-color); text-shadow: 0 0 10px rgba(34, 211, 238, 0.8); }
.action-icon-btn.btn-active .icon-wrapper { background: rgba(34, 211, 238, 0.2); border-color: rgba(34, 211, 238, 0.8); box-shadow: 0 0 25px rgba(34, 211, 238, 0.5), inset 0 0 15px rgba(34, 211, 238, 0.4); animation: active-pulse 2s infinite alternate; }
.action-icon-btn.btn-active .btn-label { color: var(--accent-color); text-shadow: 0 0 10px rgba(34, 211, 238, 0.8); font-weight: 700; }

@keyframes active-pulse { 0% { box-shadow: 0 0 20px rgba(34, 211, 238, 0.4), inset 0 0 10px rgba(34, 211, 238, 0.3); } 100% { box-shadow: 0 0 35px rgba(34, 211, 238, 0.7), inset 0 0 20px rgba(34, 211, 238, 0.6); } }

/* =========================================
   🎚️ 滑块与情绪芯片组
   ========================================= */
.slider-row { display: flex; align-items: center; gap: 12px; margin-bottom: 10px;}
.axis { font-size: 14px; font-weight: 600; color: var(--accent-color); width: 15px;}
.cyber-slider { flex: 1; margin: 0; }
.chip-grid { display: flex; flex-wrap: wrap; gap: 10px; }
.chip-btn { flex: 1 1 auto; background-color: var(--btn-bg); color: var(--text-main); border: 1px solid var(--border-color); border-radius: 12px 6px 12px 6px; font-size: 13px; padding: 8px 16px; margin: 0; transition: all 0.2s;}
.chip-btn::after { display: none; }
.chip-btn:active { background: rgba(34, 211, 238, 0.15); transform: scale(0.95); border-color: var(--accent-color); color: var(--accent-color); box-shadow: 0 0 15px rgba(34, 211, 238, 0.3);}
</style>