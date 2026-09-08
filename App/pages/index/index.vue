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

      <view class="top-grid">
        <view class="bento-item glass-card status-card-wide">
          <view class="card-top-bar">
            <view class="status-header">
              <view :class="['status-dot', isConnected ? 'dot-online' : 'dot-offline']"></view>
              <text class="card-title" style="margin-bottom: 0;">BLE / WiFi Link Status</text>
            </view>
            <view class="link-status-pills">
              <view class="link-pill">
                <view :class="['mini-status-dot', isConnected ? 'dot-online' : 'dot-offline']"></view>
                <text>BLE</text>
              </view>
              <view class="link-pill wifi-pill" @click.stop="openWifiDrawer">
                <view class="wifi-signal-icon"><view></view><view></view><view></view></view>
                <text>WiFi 配置</text>
              </view>
            </view>
          </view>

          <view class="card-bottom-bar">
            <view class="status-main">
              <text class="main-title">LingMou Core</text>
              <text class="sub-title">{{ statusDetailText }}</text>
              <text class="status-meta" v-if="wifiIp">IP · {{ wifiIp }}</text>
            </view>
            <button :class="['ble-scan-btn', isScanning ? 'btn-scanning' : '']" @click="toggleBleConnection">
              <text class="btn-text">{{ isConnected ? '断开蓝牙' : (isScanning ? '寻呼中...' : '寻呼设备') }}</text>
            </button>
          </view>
        </view>

        <view class="bento-item glass-card mode-toggle-card" @click="toggleMode">
          <view class="mode-icon-scene">
            <view :class="['mode-icon-flipper', activeMode === 'info' ? 'is-info' : '']">
              <view class="mode-icon-face mode-icon-eye">◉</view>
              <view class="mode-icon-face mode-icon-info">◌</view>
            </view>
            <view class="mode-icon-orbit"></view>
          </view>
          <text class="mode-kicker">VIEW MODE</text>
          <text class="mode-label">{{ activeMode === 'eye' ? '眼睛模式' : '信息模式' }}</text>
          <text class="mode-hint">点击卡片切换</text>
        </view>
      </view>

      <view :class="['flip-stage', activeMode === 'info' ? 'is-info' : '']">
        <view class="flip-inner">
          <!-- 眼睛卡片：快捷指令、追踪和情绪均属于同一张卡片 -->
          <view class="bento-item glass-card mode-face eye-face">
            <view class="face-heading">
              <view>
                <text class="face-kicker">EYE CONTROL</text>
                <text class="face-title">灵眸控制中心</text>
              </view>
              <text class="face-state">{{ isConnected ? 'LIVE' : 'OFFLINE' }}</text>
            </view>

            <view class="face-section command-section">
              <view class="command-grid">
                <button class="action-icon-btn blink-btn" @click="sendBlink">
                  <view class="icon-wrapper"><view class="icon-3d icon-blink"><view class="blink-core"></view><view class="blink-spark spark-a"></view><view class="blink-spark spark-b"></view></view></view>
                  <text class="btn-label">灵瞬</text>
                </button>

                <button :class="['action-icon-btn', 'auto-btn', isIllusionOn ? 'btn-active' : '']" @click="toggleIllusion">
                  <view class="icon-wrapper"><view class="icon-3d icon-illusion"><view class="mask-face mask-back"></view><view class="mask-face mask-front"></view></view></view>
                  <text class="btn-label">幻相</text>
                </button>

                <button :class="['action-icon-btn', 'manual-btn', isWanderOn ? 'btn-active' : '']" @click="toggleWander">
                  <view class="icon-wrapper"><view class="icon-3d icon-wander"><view class="wander-orbit"></view><view class="wander-core"></view><view class="wander-arrow"></view></view></view>
                  <text class="btn-label">游荡</text>
                </button>
              </view>
            </view>

            <view class="face-section tracking-section">
              <view class="section-heading">
                <text class="card-title">Eye Tracking</text>
                <text class="section-caption">手动凝视</text>
              </view>
              <view class="slider-row">
                <text class="axis">X</text>
                <slider class="cyber-slider" :value="lookX" min="-1" max="1" step="0.1" @change="onLookXChange" :activeColor="isDarkMode ? '#58A6FF' : '#0891b2'" block-size="16" />
                <text class="axis-value">{{ Number(lookX).toFixed(1) }}</text>
              </view>
              <view class="slider-row">
                <text class="axis">Y</text>
                <slider class="cyber-slider" :value="lookY" min="-1" max="1" step="0.1" @change="onLookYChange" :activeColor="isDarkMode ? '#58A6FF' : '#0891b2'" block-size="16" />
                <text class="axis-value">{{ Number(lookY).toFixed(1) }}</text>
              </view>
            </view>

            <view class="face-section emotion-section">
              <view class="section-heading">
                <text class="card-title">Emotions</text>
                <text class="section-caption">选择表情</text>
              </view>
              <view class="chip-grid">
                <button class="chip-btn" v-for="emo in emotions" :key="emo" @click="sendEmotion(emo)">
                  {{ emo }}
                </button>
              </view>
            </view>
          </view>

          <!-- 信息卡片：与眼睛卡片共用外框尺寸，翻转时不发生跳动 -->
          <view class="bento-item glass-card mode-face info-face">
            <view class="face-heading info-heading">
              <view>
                <text class="face-kicker">ENVIRONMENT</text>
                <text class="face-title">LingMou Ambient</text>
              </view>
              <view class="live-badge">
                <view :class="['mini-status-dot', telemetrySource === 'WiFi' ? 'dot-online' : 'dot-pending']"></view>
                <text>LIVE · {{ telemetrySource }}</text>
              </view>
            </view>

            <view class="info-time-row">
              <text class="info-time">{{ telemetry.time || '--:--:--' }}</text>
              <text class="info-time-note">实时环境读数</text>
            </view>

            <view class="info-metric metric-temperature">
              <view class="metric-topline">
                <view class="metric-icon thermometer-mark"><view></view></view>
                <view class="metric-copy">
                  <text class="metric-label">TEMPERATURE</text>
                  <text class="metric-note">环境温度 · {{ temperatureComfort }}</text>
                </view>
                <view class="metric-value">
                  <text class="metric-number">{{ temperatureDisplay }}</text>
                  <text class="metric-unit">°C</text>
                </view>
              </view>
              <view class="metric-track"><view class="metric-fill temp-fill" :style="{ width: temperatureLevel + '%' }"></view></view>
            </view>

            <view class="info-metric metric-humidity">
              <view class="metric-topline">
                <view class="metric-icon droplet-mark"><view></view></view>
                <view class="metric-copy">
                  <text class="metric-label">HUMIDITY</text>
                  <text class="metric-note">空气湿度 · {{ humidityComfort }}</text>
                </view>
                <view class="metric-value">
                  <text class="metric-number">{{ humidityDisplay }}</text>
                  <text class="metric-unit">%</text>
                </view>
              </view>
              <view class="metric-track"><view class="metric-fill humidity-fill" :style="{ width: humidityLevel + '%' }"></view></view>
            </view>

            <view class="info-footer">
              <text>AHT20 · {{ telemetry.aht ? 'READY' : 'UNAVAILABLE' }}</text>
              <text>{{ wifiConnected ? 'WiFi LIVE' : 'BLE LIVE' }}</text>
            </view>
          </view>
        </view>
      </view>

      <view v-if="wifiDrawerVisible" class="drawer-layer" @click="closeWifiDrawer">
        <view class="wifi-drawer" @click.stop>
          <view class="drawer-handle"></view>
          <view class="drawer-heading">
            <view>
              <text class="face-kicker">NETWORK SETUP</text>
              <text class="drawer-title">WiFi 配置</text>
            </view>
            <text class="drawer-close" @click="closeWifiDrawer">×</text>
          </view>

          <view class="drawer-field">
            <text class="drawer-label">WiFi 名称</text>
            <input class="drawer-input" v-model="wifiForm.ssid" placeholder="输入 2.4GHz WiFi 名称" />
          </view>
          <view class="drawer-field">
            <text class="drawer-label">WiFi 密码</text>
            <input class="drawer-input" v-model="wifiForm.password" password placeholder="输入 WiFi 密码" />
          </view>

          <view :class="['drawer-status-row', 'drawer-status-' + wifiConfigState]">
            <view :class="['mini-status-dot', wifiConnected ? 'dot-online' : (wifiConnecting ? 'dot-pending' : 'dot-offline')]"></view>
            <text>{{ wifiDrawerStatus }}</text>
            <text v-if="wifiIp" class="drawer-ip">{{ wifiIp }}</text>
          </view>

          <view class="drawer-actions">
            <button class="drawer-secondary-btn" @click="closeWifiDrawer">取消</button>
            <button :class="['drawer-primary-btn', wifiConfigSaving ? 'is-loading' : '', wifiConfigState === 'success' ? 'is-success' : '', wifiConfigState === 'error' ? 'is-error' : '']" :disabled="wifiConfigSaving" @click="saveWifiConfig">
              <view v-if="wifiConfigSaving" class="button-spinner"></view>
              <text>{{ wifiConfigState === 'success' ? '已连接' : (wifiConfigState === 'error' ? '重试连接' : (wifiConfigSaving ? '正在发送...' : '保存并连接')) }}</text>
            </button>
          </view>
          <view class="wifi-flow">
            <view :class="['wifi-flow-step', isConnected ? 'is-done' : '']"><view class="flow-dot"></view><text>BLE</text></view>
            <view class="flow-line"></view>
            <view :class="['wifi-flow-step', wifiConfigState === 'sending' || wifiConfigState === 'waiting' || wifiConnected ? 'is-done' : '']"><view class="flow-dot"></view><text>配网</text></view>
            <view class="flow-line"></view>
            <view :class="['wifi-flow-step', wifiConnected ? 'is-done' : '']"><view class="flow-dot"></view><text>在线</text></view>
          </view>
          <text class="drawer-note">配网信息通过已连接的 BLE 通道发送到 LingMou</text>
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
      txCharId: 'beb5483e-36e1-4688-b7f5-ea07361b26a9',  // ESP32 遥测通知特征值 UUID
      targetDeviceName: 'LingMou', // 锁定寻找名为 LingMou 的设备
      targetDeviceId: '',          // 成功识别后记住设备地址；名称缺失时也可再次识别
      bleScanMode: 'idle',         // idle | service | fallback
      
      // 🔁 正反两面主卡片
      activeMode: 'eye',
      bleRxBuffer: '',

      // 🌡️ 硬件遥测
      telemetry: {
        temperature: null,
        humidity: null,
        aht: 0,
        time: '--:--:--',
        wifi: 0
      },
      telemetrySource: 'BLE',
      wifiConnected: false,
      wifiConnecting: false,
      wifiSsid: '',
      wifiIp: '',
      wifiDrawerVisible: false,
      wifiConfigSaving: false,
      wifiConfigState: 'idle',
      wifiForm: {
        ssid: '',
        password: ''
      },
      wifiPollTimer: null,
      wifiBleStatusTimer: null,
      wifiBleStatusDeadline: 0,
      modeFlipTimer: null,
      bleConnectionListenerReady: false,
      bleValueListenerReady: false,
      bleAdapterReady: false,
      bleFoundListenerReady: false,
      bleInitInProgress: false,
      scanTimeoutTimer: null,

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
  computed: {
    statusDetailText() {
      if (!this.isConnected) return this.statusText;
      if (this.wifiConnected) return '蓝牙已连接 · WiFi 已连接';
      if (this.wifiConnecting) return '蓝牙已连接 · WiFi 连接中';
      return '蓝牙已连接 · WiFi 待配置';
    },
    temperatureDisplay() {
      return this.telemetry.temperature === null || this.telemetry.temperature === undefined
        ? '--.-'
        : Number(this.telemetry.temperature).toFixed(1);
    },
    humidityDisplay() {
      return this.telemetry.humidity === null || this.telemetry.humidity === undefined
        ? '--.-'
        : Number(this.telemetry.humidity).toFixed(1);
    },
    temperatureLevel() {
      const value = Number(this.telemetry.temperature);
      if (!Number.isFinite(value)) return 0;
      return Math.max(4, Math.min(100, ((value + 10) / 50) * 100));
    },
    humidityLevel() {
      const value = Number(this.telemetry.humidity);
      if (!Number.isFinite(value)) return 0;
      return Math.max(4, Math.min(100, value));
    },
    temperatureComfort() {
      const value = Number(this.telemetry.temperature);
      if (!Number.isFinite(value)) return '等待数据';
      if (value < 18) return '偏低';
      if (value > 28) return '偏高';
      return '舒适';
    },
    humidityComfort() {
      const value = Number(this.telemetry.humidity);
      if (!Number.isFinite(value)) return '等待数据';
      if (value < 35) return '偏干';
      if (value > 70) return '偏湿';
      return '舒适';
    },
    wifiDrawerStatus() {
      if (this.wifiConfigState === 'sending') return '正在通过 BLE 发送配置...';
      if (this.wifiConfigState === 'waiting') return '设备正在连接 WiFi...';
      if (this.wifiConfigState === 'success') return 'WiFi 已连接，可开始读取数据';
      if (this.wifiConfigState === 'error') return '连接失败，请检查名称和密码';
      if (this.wifiConnected) return 'WiFi 已连接';
      if (this.wifiConnecting) return 'WiFi 连接中...';
      if (!this.isConnected) return '请先连接 BLE';
      return '等待 WiFi 配置';
    }
  },
  onLoad() {
    setTimeout(() => { this.showSplash = false; }, 4000); 
    this.syncSystemTheme(); 
    // 自动初始化蓝牙模块
    this.initBluetooth();
  },
  onUnload() {
    this.stopWifiPolling();
    this.stopBleWifiStatusPolling();
    if (this.modeFlipTimer) clearTimeout(this.modeFlipTimer);
    if (this.scanTimeoutTimer) {
      clearTimeout(this.scanTimeoutTimer);
      this.scanTimeoutTimer = null;
    }
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
    getMissingBleApis() {
      const requiredApis = [
        'openBluetoothAdapter',
        'startBluetoothDevicesDiscovery',
        'stopBluetoothDevicesDiscovery',
        'onBluetoothDeviceFound',
        'createBLEConnection',
        'getBLEDeviceServices',
        'getBLEDeviceCharacteristics',
        'notifyBLECharacteristicValueChange',
        'writeBLECharacteristicValue'
      ];
      return requiredApis.filter((name) => typeof uni[name] !== 'function');
    },

    showBleRuntimeError(missingApis) {
      const missing = missingApis || this.getMissingBleApis();
      console.error('BLE API unavailable:', missing);

      // H5/Web 没有 uni-app BLE API。App 中出现这个错误通常是 Bluetooth 模块
      // 没有打进基座/安装包，需要在 manifest.json 的 App模块配置中启用 Bluetooth。
      this.isScanning = false;
      this.bleAdapterReady = false;
      this.statusText = '当前运行环境没有 BLE 能力';

      uni.showModal({
        title: 'BLE 功能不可用',
        content:
          '当前运行环境缺少 ' + (missing[0] || 'BLE') +
          '。如果正在浏览器/H5调试，请改为运行到 Android/iOS 真机；' +
          '如果已经是真机 App，请在 manifest.json → App模块配置中勾选 Bluetooth(低功耗蓝牙)，然后重新制作自定义基座或重新打包安装。',
        showCancel: false
      });
    },

    requestAndroidBlePermissions(done) {
      // #ifdef APP-PLUS
      const sys = uni.getSystemInfoSync();
      const platform = String(sys.platform || '').toLowerCase();

      if (platform === 'android' && typeof plus !== 'undefined' && plus.android) {
        const versionText = String(sys.system || (plus.os && plus.os.version) || '');
        const versionMatch = versionText.match(/(\d+)/);
        const androidMajor = versionMatch ? Number(versionMatch[1]) : 0;

        // Android 12+ 使用“附近的设备”权限；Android 11- BLE 扫描通常需要定位权限。
        const permissions = androidMajor >= 12
          ? [
              'android.permission.BLUETOOTH_SCAN',
              'android.permission.BLUETOOTH_CONNECT'
            ]
          : [
              'android.permission.ACCESS_FINE_LOCATION'
            ];

        plus.android.requestPermissions(
          permissions,
          (result) => {
            const deniedPresent = result.deniedPresent || [];
            const deniedAlways = result.deniedAlways || [];
            const denied = deniedPresent.concat(deniedAlways);

            if (denied.length) {
              this.statusText = '缺少蓝牙扫描权限';
              uni.showModal({
                title: '需要蓝牙权限',
                content: '请允许“附近的设备/蓝牙”权限后再搜索 LingMou。若之前选择了不再询问，请到系统设置中手动开启。',
                showCancel: false
              });
              done(false);
              return;
            }
            done(true);
          },
          (err) => {
            console.error('申请 BLE 权限失败:', err);
            this.statusText = '蓝牙权限申请失败';
            uni.showToast({ title: '蓝牙权限申请失败', icon: 'none' });
            done(false);
          }
        );
        return;
      }
      // #endif

      done(true);
    },

    openBleAdapter(done) {
      if (this.bleAdapterReady) {
        done(true);
        return;
      }

      const missingApis = this.getMissingBleApis();
      if (missingApis.length) {
        this.showBleRuntimeError(missingApis);
        done(false);
        return;
      }

      if (this.bleInitInProgress) {
        setTimeout(() => this.openBleAdapter(done), 250);
        return;
      }

      this.bleInitInProgress = true;
      uni.openBluetoothAdapter({
        success: () => {
          this.bleInitInProgress = false;
          this.bleAdapterReady = true;
          console.log('BLE adapter ready');

          if (typeof uni.getBluetoothAdapterState === 'function') {
            uni.getBluetoothAdapterState({
              success: (state) => {
                console.log('BLE adapter state:', state);
                if (!state.available) {
                  this.bleAdapterReady = false;
                  this.statusText = '请打开手机蓝牙';
                  uni.showToast({ title: '请打开手机蓝牙', icon: 'none' });
                  done(false);
                  return;
                }
                done(true);
              },
              fail: () => done(true)
            });
          } else {
            done(true);
          }
        },
        fail: (err) => {
          this.bleInitInProgress = false;
          this.bleAdapterReady = false;
          console.error('openBluetoothAdapter failed:', err);

          const code = Number(err && err.errCode);
          if (code === 10001) {
            this.statusText = '手机蓝牙未开启';
            uni.showToast({ title: '请先打开手机蓝牙', icon: 'none' });
          } else {
            this.statusText = '蓝牙模块初始化失败';
            uni.showModal({
              title: '蓝牙初始化失败',
              content: 'errCode=' + (err && err.errCode !== undefined ? err.errCode : 'unknown') +
                ' ' + (err && err.errMsg ? err.errMsg : ''),
              showCancel: false
            });
          }
          done(false);
        }
      });
    },

    ensureBleReady(done) {
      const missingApis = this.getMissingBleApis();
      if (missingApis.length) {
        this.showBleRuntimeError(missingApis);
        done(false);
        return;
      }

      this.requestAndroidBlePermissions((permissionOk) => {
        if (!permissionOk) {
          done(false);
          return;
        }
        this.openBleAdapter(done);
      });
    },

    initBluetooth() {
      try {
        this.targetDeviceId = String(uni.getStorageSync('lingmouBleDeviceId') || '');
      } catch (e) {
        this.targetDeviceId = '';
      }

      // #ifdef H5
      this.bleAdapterReady = false;
      this.statusText = 'H5/浏览器不支持 BLE，请运行到真机 App';
      console.warn('BLE is not supported by uni-app H5/Web runtime.');
      return;
      // #endif

      this.ensureBleReady((ok) => {
        if (ok) {
          this.statusText = '蓝牙已就绪，点击寻呼设备';
          this.registerBleFoundListener();
        }
      });
    },

    toggleBleConnection() {
      if (this.isConnected) {
        this.disconnectBle();
        return;
      }

      if (this.isScanning) {
        this.stopScan();
        this.statusText = '已停止寻呼';
        return;
      }

      this.ensureBleReady((ok) => {
        if (!ok) return;
        this.scanForLingMou();
      });
    },

    registerBleFoundListener() {
      if (this.bleFoundListenerReady) return;
      if (typeof uni.onBluetoothDeviceFound !== 'function') {
        this.showBleRuntimeError(['onBluetoothDeviceFound']);
        return;
      }

      uni.onBluetoothDeviceFound((result) => {
        const deviceList = result && Array.isArray(result.devices)
          ? result.devices
          : (result && result.deviceId ? [result] : []);

        if (!deviceList.length) return;

        for (let i = 0; i < deviceList.length; i++) {
          const device = deviceList[i] || {};
          const name = String(device.localName || device.name || '').trim();
          const serviceUUIDs = Array.isArray(device.advertisServiceUUIDs)
            ? device.advertisServiceUUIDs.map((uuid) => String(uuid).toLowerCase())
            : [];

          console.log(
            'BLE found:',
            name || '(no name)',
            device.deviceId,
            'RSSI=', device.RSSI,
            'services=', serviceUUIDs
          );

          const nameMatched =
            name.toLowerCase() === this.targetDeviceName.toLowerCase();
          const serviceMatched =
            serviceUUIDs.indexOf(this.serviceId.toLowerCase()) >= 0 ||
            serviceUUIDs.some((uuid) => uuid.indexOf('4fafc201') >= 0);
          const rememberedMatched =
            !!this.targetDeviceId &&
            String(device.deviceId || '').toLowerCase() === this.targetDeviceId.toLowerCase();

          // services:[LingMou Service UUID] 会由 Android 原生扫描器先过滤。
          // 某些 HTML5+ Runtime 回调仍可能把 name/services 返回为空，
          // 所以 service 扫描阶段收到的设备直接作为 LingMou 候选。
          const nativeServiceFilterMatched = this.bleScanMode === 'service';

          if (nameMatched || serviceMatched || rememberedMatched || nativeServiceFilterMatched) {
            console.log(
              'LingMou candidate:',
              device.deviceId,
              'match=',
              nameMatched ? 'name' :
              serviceMatched ? 'advertised-service' :
              rememberedMatched ? 'remembered-id' : 'native-service-filter'
            );
            this.deviceId = device.deviceId;
            this.statusText = '已发现 LingMou，正在连接...';
            this.stopScan();
            setTimeout(() => this.connectBle(), 180);
            break;
          }
        }
      });

      this.bleFoundListenerReady = true;
    },

    scanForLingMou() {
      const missingApis = this.getMissingBleApis();
      if (missingApis.length) {
        this.showBleRuntimeError(missingApis);
        return;
      }

      if (!this.bleAdapterReady) {
        this.ensureBleReady((ok) => {
          if (ok) this.scanForLingMou();
        });
        return;
      }

      this.registerBleFoundListener();
      this.startLingMouServiceScan();
    },

    startLingMouServiceScan() {
      this.isScanning = true;
      this.bleScanMode = 'service';
      this.statusText = '正在按 LingMou 服务搜索...';

      console.log('BLE service-filter scan UUID:', this.serviceId);

      uni.startBluetoothDevicesDiscovery({
        services: [this.serviceId],
        allowDuplicatesKey: true,
        interval: 0,
        powerLevel: 'high',
        success: (res) => {
          console.log('BLE service-filter discovery started:', res);
        },
        fail: (err) => {
          console.error('BLE service-filter discovery failed:', err);
          this.isScanning = false;
          this.bleScanMode = 'idle';

          const code = Number(err && err.errCode);
          if (code === 10000) {
            this.bleAdapterReady = false;
            this.statusText = '蓝牙未初始化，正在重试';
            this.ensureBleReady((ok) => {
              if (ok) this.scanForLingMou();
            });
            return;
          }
          if (code === 10001) {
            this.statusText = '请打开手机蓝牙';
            uni.showToast({ title: '请打开手机蓝牙', icon: 'none' });
            return;
          }

          console.warn('Service-filter scan unavailable, fallback to normal scan.');
          setTimeout(() => this.startLingMouFallbackScan(), 250);
        }
      });

      if (this.scanTimeoutTimer) clearTimeout(this.scanTimeoutTimer);
      this.scanTimeoutTimer = setTimeout(() => {
        this.scanTimeoutTimer = null;
        if (!this.isScanning || this.bleScanMode !== 'service') return;

        console.warn('No device returned by service filter; switching to fallback scan.');
        uni.stopBluetoothDevicesDiscovery({
          complete: () => {
            this.isScanning = false;
            this.bleScanMode = 'idle';
            setTimeout(() => this.startLingMouFallbackScan(), 250);
          }
        });
      }, 6000);
    },

    startLingMouFallbackScan() {
      this.isScanning = true;
      this.bleScanMode = 'fallback';
      this.statusText = '正在扫描 LingMou...';

      uni.startBluetoothDevicesDiscovery({
        allowDuplicatesKey: true,
        interval: 0,
        powerLevel: 'high',
        success: (res) => {
          console.log('BLE fallback discovery started:', res);

          if (typeof uni.getBluetoothDevices === 'function') {
            setTimeout(() => {
              if (!this.isScanning || this.bleScanMode !== 'fallback') return;

              uni.getBluetoothDevices({
                success: (cacheRes) => {
                  const devices = cacheRes.devices || [];
                  for (let i = 0; i < devices.length; i++) {
                    const device = devices[i] || {};
                    const name = String(device.localName || device.name || '').trim();
                    const rememberedMatched =
                      !!this.targetDeviceId &&
                      String(device.deviceId || '').toLowerCase() === this.targetDeviceId.toLowerCase();

                    if (
                      name.toLowerCase() === this.targetDeviceName.toLowerCase() ||
                      rememberedMatched
                    ) {
                      console.log('Found LingMou from cache:', device);
                      this.deviceId = device.deviceId;
                      this.statusText = '已发现 LingMou，正在连接...';
                      this.stopScan();
                      setTimeout(() => this.connectBle(), 180);
                      break;
                    }
                  }
                }
              });
            }, 1000);
          }
        },
        fail: (err) => {
          console.error('BLE fallback discovery failed:', err);
          this.isScanning = false;
          this.bleScanMode = 'idle';
          this.statusText = '扫描启动失败';
          uni.showModal({
            title: 'BLE 扫描失败',
            content: 'errCode=' + (err && err.errCode !== undefined ? err.errCode : 'unknown') +
              ' ' + (err && err.errMsg ? err.errMsg : ''),
            showCancel: false
          });
        }
      });

      if (this.scanTimeoutTimer) clearTimeout(this.scanTimeoutTimer);
      this.scanTimeoutTimer = setTimeout(() => {
        this.scanTimeoutTimer = null;
        if (this.isScanning && this.bleScanMode === 'fallback') {
          this.stopScan();
          this.statusText = '未发现 LingMou，请确认设备正在广播';
          uni.showToast({ title: '未发现 LingMou', icon: 'none' });
        }
      }, 10000);
    },

    stopScan() {
      if (this.scanTimeoutTimer) {
        clearTimeout(this.scanTimeoutTimer);
        this.scanTimeoutTimer = null;
      }

      if (typeof uni.stopBluetoothDevicesDiscovery === 'function' && this.bleAdapterReady) {
        uni.stopBluetoothDevicesDiscovery({
          complete: () => {}
        });
      }
      this.isScanning = false;
      this.bleScanMode = 'idle';
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
          this.statusText = '蓝牙已连接，正在读取服务...';
          this.bleRxBuffer = '';
          uni.showToast({ title: '蓝牙已接管！', icon: 'success' });
          this.registerBleListeners();

          // 主动协商较大 MTU，减少 ESP32 -> App 的 JSON Notify 被截断的概率。
          if (typeof uni.setBLEMTU === 'function') {
            uni.setBLEMTU({
              deviceId: this.deviceId,
              mtu: 247,
              success: (mtuRes) => console.log('BLE MTU request success:', mtuRes),
              fail: (mtuErr) => console.warn('BLE MTU request failed, continue safely:', mtuErr),
              complete: () => {
                setTimeout(() => { this.discoverLingMouService(); }, 350);
              }
            });
          } else {
            setTimeout(() => { this.discoverLingMouService(); }, 350);
          }
        },
        fail: (err) => {
          uni.hideLoading();
          this.isConnected = false;
          console.error('createBLEConnection failed:', this.deviceId, err);
          this.statusText = '连接失败：' + (err && err.errCode !== undefined ? err.errCode : 'unknown');
          uni.showToast({
            title: '连接失败 ' + (err && err.errCode !== undefined ? err.errCode : ''),
            icon: 'none'
          });
        }
      });
    },

    registerBleListeners() {
      if (!this.bleConnectionListenerReady) {
        uni.onBLEConnectionStateChange((stateRes) => {
          if (stateRes.deviceId && this.deviceId && stateRes.deviceId !== this.deviceId) return;
          if (!stateRes.connected) {
            this.isConnected = false;
            this.wifiConnecting = false;
            this.statusText = '蓝牙意外断开';
            this.stopBleWifiStatusPolling();
            this.stopWifiPolling();
            uni.showToast({ title: '连接已断开', icon: 'none' });
          }
        });
        this.bleConnectionListenerReady = true;
      }

      if (!this.bleValueListenerReady) {
        uni.onBLECharacteristicValueChange((characteristic) => {
          if (!characteristic || !characteristic.value) return;
          const characteristicId = (characteristic.characteristicId || '').toLowerCase();
          if (characteristicId !== this.txCharId.toLowerCase() && !characteristicId.endsWith('a9')) return;
          this.consumeBlePayload(this.decodeBleValue(characteristic.value));
        });
        this.bleValueListenerReady = true;
      }
    },

    discoverLingMouService() {
      uni.getBLEDeviceServices({
        deviceId: this.deviceId,
        success: (res) => {
          const target = (res.services || []).find((service) => {
            const uuid = (service.uuid || '').toLowerCase();
            return uuid === this.serviceId.toLowerCase() || uuid.includes('4fafc201');
          });
          if (!target) {
            this.statusText = '未找到 LingMou 控制服务';
            return;
          }
          this.serviceId = target.uuid;

          // GATT 服务已二次确认，这台设备就是 LingMou。
          this.targetDeviceId = this.deviceId;
          try {
            uni.setStorageSync('lingmouBleDeviceId', this.deviceId);
          } catch (e) {
            console.warn('Unable to remember LingMou deviceId:', e);
          }
          console.log('LingMou GATT service confirmed:', this.deviceId, target.uuid);

          uni.getBLEDeviceCharacteristics({
            deviceId: this.deviceId,
            serviceId: target.uuid,
            success: (cRes) => {
              const characteristics = cRes.characteristics || [];
              const writeChar = characteristics.find((item) => {
                const uuid = (item.uuid || '').toLowerCase();
                return uuid === this.charId.toLowerCase() || uuid.endsWith('a8') || item.properties && (item.properties.write || item.properties.writeNoResponse);
              });
              const notifyChar = characteristics.find((item) => {
                const uuid = (item.uuid || '').toLowerCase();
                return uuid === this.txCharId.toLowerCase() || uuid.endsWith('a9') || item.properties && (item.properties.notify || item.properties.indicate);
              });

              if (writeChar) this.charId = writeChar.uuid;
              if (!notifyChar) {
                this.statusText = '未找到遥测通知特征';
                return;
              }
              this.txCharId = notifyChar.uuid;
              this.enableTelemetryNotify(target.uuid, notifyChar.uuid);
            },
            fail: () => { this.statusText = '读取 LingMou 特征失败'; }
          });
        },
        fail: () => { this.statusText = '读取蓝牙服务失败'; }
      });
    },

    enableTelemetryNotify(serviceId, characteristicId) {
      uni.notifyBLECharacteristicValueChange({
        deviceId: this.deviceId,
        serviceId,
        characteristicId,
        state: true,
        success: () => {
          this.statusText = '物理直连已就绪';

          // 某些 Android 机型刚订阅 Notify 后立即写入会出现 10008，
          // 稍微延迟再同步时钟和状态。
          setTimeout(() => {
            this.sendBLECommand(`TIME=${Math.floor(Date.now() / 1000)}`, () => {
              setTimeout(() => this.sendBLECommand('GET_STATUS'), 180);
            });
          }, 350);

          setTimeout(() => {
            uni.readBLECharacteristicValue({
              deviceId: this.deviceId,
              serviceId,
              characteristicId
            });
          }, 500);
        },
        fail: () => { this.statusText = '通知订阅失败，可继续使用控制功能'; }
      });
    },

    decodeBleValue(value) {
      const bytes = new Uint8Array(value);
      if (typeof TextDecoder !== 'undefined') {
        try { return new TextDecoder('utf-8').decode(bytes); } catch (e) { /* fallback below */ }
      }
      let result = '';
      for (let i = 0; i < bytes.length; i++) result += String.fromCharCode(bytes[i]);
      return result;
    },

    consumeBlePayload(payload) {
      if (!payload) return;
      this.bleRxBuffer += payload;
      let start = this.bleRxBuffer.indexOf('{');
      while (start >= 0) {
        let depth = 0;
        let inString = false;
        let escaped = false;
        let end = -1;
        for (let i = start; i < this.bleRxBuffer.length; i++) {
          const ch = this.bleRxBuffer[i];
          if (escaped) { escaped = false; continue; }
          if (ch === '\\' && inString) { escaped = true; continue; }
          if (ch === '"') { inString = !inString; continue; }
          if (inString) continue;
          if (ch === '{') depth++;
          if (ch === '}') {
            depth--;
            if (depth === 0) { end = i; break; }
          }
        }
        if (end < 0) {
          if (start > 0) this.bleRxBuffer = this.bleRxBuffer.substring(start);
          break;
        }
        const frame = this.bleRxBuffer.substring(start, end + 1);
        this.bleRxBuffer = this.bleRxBuffer.substring(end + 1);
        try { this.handleBlePayload(JSON.parse(frame)); } catch (e) { console.warn('BLE 数据解析失败:', frame); }
        start = this.bleRxBuffer.indexOf('{');
      }
      if (this.bleRxBuffer.length > 1024) this.bleRxBuffer = this.bleRxBuffer.slice(-512);
    },

    handleBlePayload(data) {
      if (!data || typeof data !== 'object') return;

      console.log('BLE payload:', data);

      if (data.telemetry && typeof data.telemetry === 'object') {
        this.handleBlePayload(data.telemetry);
      }

      if (Object.prototype.hasOwnProperty.call(data, 't') || Object.prototype.hasOwnProperty.call(data, 'h')) {
        this.telemetry = {
          temperature: data.t === null ? null : Number(data.t),
          humidity: data.h === null ? null : Number(data.h),
          aht: Number(data.aht || 0),
          time: data.time || '--:--:--',
          wifi: Number(data.wifi || 0)
        };
        this.telemetrySource = this.wifiConnected ? 'WiFi' : 'BLE';
      }

      if (data.ssid !== undefined) this.wifiSsid = data.ssid || '';
      if (data.ip !== undefined && data.ip) this.wifiIp = data.ip;

      const hasWifiFlag = data.wifi !== undefined;
      const wifiUp = hasWifiFlag && Number(data.wifi) === 1;
      const hasConnectingFlag = data.connecting !== undefined;
      const connecting = hasConnectingFlag && Number(data.connecting) === 1;

      if (hasWifiFlag) this.wifiConnected = wifiUp;
      if (hasConnectingFlag) this.wifiConnecting = connecting;

      if (data.type === 'wifi') {
        if (data.state === 'connecting') {
          this.wifiConnected = false;
          this.wifiConnecting = true;
          this.wifiConfigState = 'waiting';
        }

        if (Number(data.ok) === 0) {
          this.wifiConnected = false;
          this.wifiConnecting = false;
          this.wifiConfigState = 'error';
          this.wifiConfigSaving = false;
          this.stopBleWifiStatusPolling();
        }

        if (Number(data.ok) === 1 && data.ip) {
          this.wifiConnected = true;
          this.wifiConnecting = false;
          this.wifiConfigState = 'success';
          this.wifiConfigSaving = false;
          this.stopBleWifiStatusPolling();
        }
      }

      // 关键：GET_STATUS 没有 type:"wifi"，但 wifi=1 + ip 已足以确认设备联网。
      // 手机本身作为热点时，即使 HTTP 访问热点客户端失败，也不应一直显示“连接中”。
      if (this.wifiConnected && this.wifiIp) {
        this.wifiConnecting = false;
        this.wifiConfigState = 'success';
        this.wifiConfigSaving = false;
        this.stopBleWifiStatusPolling();

        // WiFi HTTP 是增强通道；不可达时仍继续用 BLE 收遥测。
        this.startWifiPolling();

        if (this.wifiDrawerVisible) {
          setTimeout(() => {
            if (this.wifiConfigState === 'success') this.wifiDrawerVisible = false;
          }, 900);
        }
      } else if (this.wifiConnecting || connecting) {
        this.wifiConfigState = 'waiting';
      }

      if (data.state === 'cleared') {
        this.wifiConnected = false;
        this.wifiConnecting = false;
        this.wifiIp = '';
        this.wifiConfigState = 'idle';
        this.stopBleWifiStatusPolling();
        this.stopWifiPolling();
      }
    },

    disconnectBle() {
      if (this.deviceId) {
        uni.closeBLEConnection({
          deviceId: this.deviceId,
          success: () => {
            this.isConnected = false;
            this.statusText = '蓝牙已断开';
            this.wifiConnecting = false;
            this.stopBleWifiStatusPolling();
            this.stopWifiPolling();
          }
        });
      }
    },

    closeBluetooth() {
      if (this.scanTimeoutTimer) {
        clearTimeout(this.scanTimeoutTimer);
        this.scanTimeoutTimer = null;
      }
      this.stopScan();
      this.disconnectBle();

      if (typeof uni.closeBluetoothAdapter === 'function' && this.bleAdapterReady) {
        uni.closeBluetoothAdapter({
          complete: () => {
            this.bleAdapterReady = false;
          }
        });
      } else {
        this.bleAdapterReady = false;
      }
    },

    toggleMode() {
      this.activeMode = this.activeMode === 'eye' ? 'info' : 'eye';
    },

    openWifiDrawer() {
      this.wifiForm.ssid = this.wifiSsid;
      this.wifiForm.password = '';
      if (!this.wifiConnected && !this.wifiConnecting) this.wifiConfigState = 'idle';
      this.wifiDrawerVisible = true;
    },

    closeWifiDrawer() {
      this.wifiDrawerVisible = false;
    },

    encodeUtf8(text) {
      if (typeof TextEncoder !== 'undefined') return Array.from(new TextEncoder().encode(text));
      const encoded = unescape(encodeURIComponent(text));
      const bytes = [];
      for (let i = 0; i < encoded.length; i++) bytes.push(encoded.charCodeAt(i));
      return bytes;
    },

    splitUtf8Text(text, maxBytes) {
      const chunks = [];
      let current = '';
      for (const character of text) {
        const candidate = current + character;
        if (current && this.encodeUtf8(candidate).length > maxBytes) {
          chunks.push(current);
          current = character;
        } else {
          current = candidate;
        }
      }
      if (current || !chunks.length) chunks.push(current);
      return chunks;
    },

    sendWifiField(prefix, appendPrefix, value, done) {
      // 预留 BLE ATT 包头空间，兼容未完成 MTU 协商的手机。
      const chunks = this.splitUtf8Text(value || '', 8);
      const commands = chunks.map((chunk, index) => `${index === 0 ? prefix : appendPrefix}${chunk}`);
      let index = 0;
      const next = () => {
        if (index >= commands.length) { if (done) done(); return; }
        this.sendBLECommand(commands[index++], () => setTimeout(next, 90));
      };
      next();
    },

    saveWifiConfig() {
      if (!this.isConnected) {
        uni.showToast({ title: '请先连接 BLE', icon: 'none' });
        return;
      }
      if (!this.wifiForm.ssid.trim()) {
        uni.showToast({ title: '请输入 WiFi 名称', icon: 'none' });
        return;
      }
      this.wifiConfigSaving = true;
      this.wifiConfigState = 'sending';
      this.wifiSsid = this.wifiForm.ssid.trim();
      this.wifiConnecting = true;
      this.sendWifiField('WIFI_SSID=', 'WIFI_SSID+', this.wifiSsid, () => {
        this.sendWifiField('WIFI_PASS=', 'WIFI_PASS+', this.wifiForm.password, () => {
          this.sendBLECommand('WIFI_CONNECT', () => {
            this.wifiConfigSaving = false;
            this.wifiConfigState = 'waiting';
            this.startBleWifiStatusPolling();
            uni.showToast({ title: '已发送 WiFi 配置', icon: 'success' });
          });
        });
      });
    },

    startBleWifiStatusPolling() {
      this.stopBleWifiStatusPolling();
      if (!this.isConnected) return;

      this.wifiBleStatusDeadline = Date.now() + 25000;

      const queryStatus = () => {
        if (!this.isConnected || this.wifiConfigState === 'success') {
          this.stopBleWifiStatusPolling();
          return;
        }

        if (Date.now() >= this.wifiBleStatusDeadline) {
          this.stopBleWifiStatusPolling();
          if (!this.wifiConnected) {
            this.wifiConnecting = false;
            this.wifiConfigState = 'error';
            this.wifiConfigSaving = false;
            uni.showToast({ title: '未收到设备联网状态', icon: 'none' });
          }
          return;
        }

        this.sendBLECommand('GET_STATUS');
      };

      setTimeout(queryStatus, 800);
      this.wifiBleStatusTimer = setInterval(queryStatus, 1200);
    },

    stopBleWifiStatusPolling() {
      if (this.wifiBleStatusTimer) {
        clearInterval(this.wifiBleStatusTimer);
        this.wifiBleStatusTimer = null;
      }
      this.wifiBleStatusDeadline = 0;
    },

    startWifiPolling() {
      if (!this.wifiIp || this.wifiPollTimer) return;
      this.pollWifiTelemetry();
      this.wifiPollTimer = setInterval(() => this.pollWifiTelemetry(), 3000);
    },

    stopWifiPolling() {
      if (this.wifiPollTimer) clearInterval(this.wifiPollTimer);
      this.wifiPollTimer = null;
    },

    pollWifiTelemetry() {
      if (!this.wifiIp) return;
      uni.request({
        url: `http://${this.wifiIp}/api/telemetry`,
        timeout: 2500,
        success: (res) => {
          let payload = res.data;
          if (typeof payload === 'string') {
            try { payload = JSON.parse(payload); } catch (e) { return; }
          }
          if (!payload || typeof payload !== 'object') return;
          this.wifiConnected = true;
          this.wifiConfigState = 'success';
          this.wifiConfigSaving = false;
          this.telemetrySource = 'WiFi';
          this.handleBlePayload(payload);
        },
        fail: (err) => {
          // 同一台手机作为热点时，部分 ROM 会限制“热点宿主 -> 热点客户端”的访问；
          // 也可能是 Android 明文 HTTP 策略阻止请求。BLE 状态仍可确认 WiFi 已连接。
          console.warn('WiFi HTTP telemetry unavailable, keep BLE fallback:', err);
          this.telemetrySource = 'BLE';
        }
      });
    },

    // ==========================================
    // 🧬 极简 MTU 发射协议 (短包高速传输)
    // ==========================================
    sendBLECommand(cmdString, onComplete) {
      if (!this.isConnected) {
        uni.showToast({ title: '蓝牙未连接', icon: 'none' });
        if (onComplete) onComplete();
        return;
      }
      
      const encoded = this.encodeUtf8(cmdString);
      const buffer = new ArrayBuffer(encoded.length);
      const dataView = new Uint8Array(buffer);
      dataView.set(encoded);

      // 🔴 使用你原来定义的 UUID 变量即可，因为这是我们自定义的 128 位 UUID，不会有平台差异
      uni.writeBLECharacteristicValue({
        deviceId: this.deviceId,
        serviceId: this.serviceId,
        characteristicId: this.charId,
        value: buffer,
        writeType: 'writeNoResponse', // 🚀 极速盲发模式，极大降低通信延迟
        success: () => {
          if (!/^WIFI_PASS/.test(cmdString)) console.log('指令击中:', cmdString);
          if (onComplete) onComplete();
        },
        fail: (err) => {
          console.error('指令发射失败:', err);
          if (onComplete) onComplete();
        }
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
  --color-black: #0D1117;
  --color-carbon: #161B22;
  --color-link-blue: #58A6FF;
  --color-text-gray: #C9D1D9;
  --bg-color: #0D1117;
  --card-bg: rgba(22, 27, 34, 0.92);
  --text-main: #C9D1D9;
  --text-sub: #8B949E;
  --border-color: rgba(201, 209, 217, 0.14);
  --btn-bg: #161B22;
  --btn-hover: #21262D;
  --accent-color: #58A6FF;
  --gradient-start: #161B22;
  --gradient-end: #58A6FF;
  --shadow-color: rgba(0, 0, 0, 0.46);
  --orb-color: rgba(88, 166, 255, 0.08);
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
  border: none;
  border-radius: 20px;
  padding: 0 14px;
  width: 104px;
  min-width: 104px;
  height: 38px;
  min-height: 38px;
  flex: 0 0 104px;
  flex-shrink: 0;
  box-sizing: border-box;
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
  white-space: nowrap;
  line-height: 1;
  box-shadow: 0 4px 15px rgba(8, 145, 178, 0.3);
  transition: all 0.3s;
  margin: 0;
}
.ble-scan-btn::after { display: none; }
.ble-scan-btn:active { transform: scale(0.95); }
.btn-scanning { animation: pulse-btn 1.5s infinite alternate; background: var(--btn-bg); border: 1px solid var(--border-color); box-shadow: none;}
.btn-scanning .btn-text { color: var(--text-main); }
.btn-text {
  display: block;
  width: 100%;
  font-size: 13px;
  font-weight: 600;
  line-height: 38px;
  text-align: center;
  color: white;
  white-space: nowrap;
  word-break: keep-all;
  overflow-wrap: normal;
}
.status-main {
  flex: 1 1 auto;
  min-width: 0;
  overflow: hidden;
}
.card-bottom-bar {
  gap: 12px;
}

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

/* =========================================
   🪞 新版主布局：2/3 连接区 + 1/3 模式区
   ========================================= */
.container { max-width: 720px; }
.top-grid { display: grid; grid-template-columns: minmax(0, 2fr) minmax(150px, 1fr); gap: 16px; align-items: stretch; margin-bottom: 18px; }
.top-grid .status-card-wide { grid-column: auto; min-height: 154px; box-sizing: border-box; }
.link-status-pills { display: flex; align-items: center; gap: 8px; }
.link-pill { display: inline-flex; align-items: center; gap: 5px; padding: 5px 9px; border: 1px solid var(--border-color); border-radius: 999px; color: var(--text-sub); font-size: 10px; letter-spacing: 0.6px; background: var(--btn-bg); }
.wifi-pill { cursor: pointer; transition: transform 0.25s ease, border-color 0.25s ease; }
.wifi-pill:hover { transform: translateY(-1px); border-color: var(--accent-color); }
.wifi-pill:active { transform: translateY(2px) scale(0.97); border-color: var(--accent-color); }
.mini-status-dot { width: 7px; height: 7px; border-radius: 50%; flex-shrink: 0; transition: all 0.35s ease; }
.dot-pending { background: #f59e0b; box-shadow: 0 0 9px rgba(245, 158, 11, 0.55); }
.status-meta { display: block; margin-top: 5px; color: var(--accent-color); font-size: 10px; letter-spacing: 0.5px; }

/* 模式卡片：整张卡片可点击，图标像主题按钮一样切换 */
.mode-toggle-card { min-height: 154px; padding: 18px 16px; border-radius: 12px 30px 12px 30px; display: flex; flex-direction: column; justify-content: center; align-items: center; text-align: center; cursor: pointer; overflow: hidden; transition: transform 0.35s ease, border-color 0.35s ease, box-shadow 0.35s ease; }
.mode-toggle-card:active { transform: translateY(2px) scale(0.985); }
.mode-icon-scene { width: 58px; height: 58px; position: relative; display: flex; align-items: center; justify-content: center; border-radius: 50%; background: linear-gradient(145deg, rgba(34, 211, 238, 0.2), rgba(8, 145, 178, 0.08)); border: 1px solid rgba(34, 211, 238, 0.28); box-shadow: 0 8px 20px var(--shadow-color), inset 0 0 16px rgba(34, 211, 238, 0.08); margin-bottom: 9px; }
.mode-icon-flipper { width: 30px; height: 30px; position: relative; transform-style: preserve-3d; transition: transform 0.55s cubic-bezier(0.2, 0.8, 0.2, 1); z-index: 2; }
.mode-icon-flipper.is-info { transform: rotateY(180deg); }
.mode-icon-face { position: absolute; inset: 0; display: flex; align-items: center; justify-content: center; color: var(--accent-color); font-size: 27px; line-height: 1; backface-visibility: hidden; -webkit-backface-visibility: hidden; text-shadow: 0 0 12px rgba(34, 211, 238, 0.45); }
.mode-icon-info { transform: rotateY(180deg); }
.mode-icon-orbit { position: absolute; inset: 7px; border: 1px solid rgba(34, 211, 238, 0.35); border-radius: 50%; transform: rotate(-22deg) scaleX(1.25); opacity: 0.7; animation: mode-orbit-spin 7s linear infinite; }
@keyframes mode-orbit-spin { from { transform: rotate(-22deg) scaleX(1.25); } to { transform: rotate(338deg) scaleX(1.25); } }
.mode-kicker { color: var(--text-sub); font-size: 9px; letter-spacing: 2px; margin-bottom: 4px; }
.mode-label { color: var(--text-main); font-size: 15px; font-weight: 700; }
.mode-hint { color: var(--text-sub); font-size: 10px; margin-top: 5px; }

/* =========================================
   🔄 主卡片翻转面：保留不规则留白和交替圆角
   ========================================= */
.flip-stage { width: 100%; perspective: 1500px; }
.flip-inner { display: grid; width: 100%; transform-style: preserve-3d; transition: transform 0.68s cubic-bezier(0.2, 0.75, 0.2, 1); }
.flip-stage.is-info .flip-inner { transform: rotateY(180deg); }
.mode-face { grid-area: 1 / 1; width: 100%; min-width: 0; box-sizing: border-box; padding: 18px; backface-visibility: hidden; -webkit-backface-visibility: hidden; overflow: hidden; }
.eye-face, .info-face { border-radius: 12px 32px 12px 32px; }
.info-face { transform: rotateY(180deg); }
.face-heading { display: flex; align-items: flex-start; justify-content: space-between; gap: 12px; margin: 3px 4px 18px; }
.face-kicker { display: block; color: var(--accent-color); font-size: 10px; font-weight: 700; letter-spacing: 2.2px; }
.face-title { display: block; color: var(--text-main); font-size: 20px; font-weight: 700; margin-top: 5px; }
.face-state { padding-top: 4px; color: var(--text-sub); font-size: 10px; letter-spacing: 1.5px; }
.face-section { background: rgba(255, 255, 255, 0.26); border: 1px solid var(--border-color); box-shadow: 0 8px 24px var(--shadow-color); }
.dark-theme .face-section { background: rgba(15, 23, 42, 0.24); }
.command-section { margin: 0 0 19px 0; padding: 20px 12px 18px; border-radius: 12px 28px 12px 28px; }
.command-grid { display: flex; align-items: flex-start; justify-content: space-evenly; gap: 8px; }
.tracking-section { margin: 0 19px 21px 0; padding: 19px 20px 16px; border-radius: 30px 12px 30px 12px; }
.emotion-section { margin: 0 0 0 17px; padding: 19px 20px 20px; border-radius: 12px 30px 12px 30px; }
.section-heading { display: flex; align-items: baseline; justify-content: space-between; gap: 12px; margin-bottom: 14px; }
.section-heading .card-title { margin-bottom: 0; }
.section-caption { color: var(--text-sub); font-size: 10px; letter-spacing: 0.6px; }
.axis-value { width: 27px; color: var(--text-sub); font-size: 11px; text-align: right; font-variant-numeric: tabular-nums; }
.tracking-section .slider-row { margin-bottom: 13px; }
.tracking-section .slider-row:last-child { margin-bottom: 0; }

/* 信息卡片：上下两行指标，保持充分留白 */
.info-heading { margin-bottom: 13px; }
.live-badge { display: inline-flex; align-items: center; gap: 6px; color: var(--text-sub); font-size: 10px; letter-spacing: 0.7px; padding-top: 4px; white-space: nowrap; }
.info-time-row { display: flex; align-items: baseline; justify-content: space-between; margin: 0 16px 19px 2px; padding-bottom: 14px; border-bottom: 1px solid var(--border-color); }
.info-time { color: var(--text-main); font-size: 31px; font-weight: 300; letter-spacing: 1px; font-variant-numeric: tabular-nums; }
.info-time-note { color: var(--text-sub); font-size: 10px; }
.info-metric { padding: 22px 20px 19px; background: rgba(255, 255, 255, 0.26); border: 1px solid var(--border-color); box-shadow: 0 10px 28px var(--shadow-color); }
.dark-theme .info-metric { background: rgba(15, 23, 42, 0.24); }
.metric-temperature { margin: 0 18px 0 0; border-radius: 30px 12px 30px 12px; }
.metric-humidity { margin: 21px 0 0 18px; border-radius: 12px 30px 12px 30px; }
.metric-topline { display: flex; align-items: center; gap: 13px; min-height: 54px; }
.metric-icon { width: 37px; height: 37px; border-radius: 13px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; background: var(--btn-bg); border: 1px solid var(--border-color); position: relative; }
.thermometer-mark:before { content: ''; width: 8px; height: 19px; border: 2px solid #fb923c; border-radius: 8px; position: absolute; top: 6px; left: 13px; }
.thermometer-mark:after { content: ''; width: 13px; height: 13px; border-radius: 50%; background: #fb923c; position: absolute; bottom: 5px; left: 10.5px; box-shadow: 0 0 10px rgba(251, 146, 60, 0.35); }
.thermometer-mark > view { width: 4px; height: 14px; border-radius: 4px; background: #fb923c; position: absolute; left: 15px; top: 9px; z-index: 2; }
.droplet-mark:before { content: ''; width: 17px; height: 22px; background: rgba(34, 211, 238, 0.18); border: 2px solid #22d3ee; border-radius: 60% 60% 65% 65%; transform: rotate(45deg) scale(0.72); position: absolute; top: 7px; left: 9px; }
.droplet-mark:after { content: ''; width: 7px; height: 7px; border-radius: 50%; background: #22d3ee; position: absolute; top: 14px; left: 15px; box-shadow: 0 0 10px rgba(34, 211, 238, 0.4); }
.metric-copy { min-width: 0; flex: 1; }
.metric-label { display: block; color: var(--text-sub); font-size: 10px; font-weight: 700; letter-spacing: 1.5px; }
.metric-note { display: block; color: var(--text-sub); font-size: 10px; margin-top: 5px; }
.metric-value { display: flex; align-items: baseline; gap: 4px; flex-shrink: 0; }
.metric-number { color: var(--text-main); font-size: 29px; font-weight: 300; font-variant-numeric: tabular-nums; }
.metric-unit { color: var(--text-sub); font-size: 12px; }
.metric-track { height: 4px; background: var(--border-color); border-radius: 99px; overflow: hidden; margin-top: 17px; }
.metric-fill { height: 100%; border-radius: inherit; transition: width 0.6s ease; }
.temp-fill { background: linear-gradient(90deg, #fbbf24, #fb923c); }
.humidity-fill { background: linear-gradient(90deg, #38bdf8, #22d3ee); }
.info-footer { display: flex; justify-content: space-between; gap: 12px; margin: 21px 4px 2px; color: var(--text-sub); font-size: 10px; letter-spacing: 0.6px; }

/* =========================================
   🛜 WiFi 底部抽屉
   ========================================= */
.drawer-layer { position: fixed; inset: 0; z-index: 100000; display: flex; align-items: flex-end; background: rgba(2, 6, 23, 0.36); animation: drawer-fade-in 0.25s ease-out; }
.wifi-drawer { width: 100%; box-sizing: border-box; padding: 12px 22px 24px; border-radius: 30px 30px 0 0; background: var(--bg-color); color: var(--text-main); box-shadow: 0 -12px 40px rgba(15, 23, 42, 0.22); animation: drawer-rise-in 0.35s cubic-bezier(0.2, 0.8, 0.2, 1); }
.drawer-handle { width: 42px; height: 4px; margin: 0 auto 20px; border-radius: 99px; background: var(--border-color); }
.drawer-heading { display: flex; align-items: flex-start; justify-content: space-between; margin-bottom: 20px; }
.drawer-title { display: block; color: var(--text-main); font-size: 24px; font-weight: 700; margin-top: 4px; }
.drawer-close { color: var(--text-sub); font-size: 28px; line-height: 20px; padding: 5px 7px; }
.drawer-field { margin-bottom: 14px; }
.drawer-label { display: block; color: var(--text-sub); font-size: 11px; margin: 0 0 7px 3px; }
.drawer-input { width: 100%; height: 44px; box-sizing: border-box; padding: 0 14px; border: 1px solid var(--border-color); border-radius: 12px 22px 12px 22px; background: var(--btn-bg); color: var(--text-main); font-size: 13px; }
.drawer-status-row { display: flex; align-items: center; gap: 8px; min-height: 22px; color: var(--text-sub); font-size: 11px; }
.drawer-ip { margin-left: auto; color: var(--accent-color); font-variant-numeric: tabular-nums; }
.drawer-actions { display: flex; gap: 12px; margin-top: 18px; }
.drawer-actions button { flex: 1; height: 42px; margin: 0; border-radius: 14px 24px 14px 24px; font-size: 13px; }
.drawer-actions button::after { display: none; }
.drawer-secondary-btn { color: var(--text-main); background: var(--btn-bg); border: 1px solid var(--border-color); }
.drawer-primary-btn { color: #fff; background: linear-gradient(135deg, var(--gradient-start), var(--gradient-end)); border: none; box-shadow: 0 6px 18px rgba(8, 145, 178, 0.25); }
.drawer-primary-btn[disabled] { opacity: 0.55; }
.drawer-note { display: block; margin-top: 14px; color: var(--text-sub); font-size: 10px; text-align: center; }
@keyframes drawer-fade-in { from { opacity: 0; } to { opacity: 1; } }
@keyframes drawer-rise-in { from { opacity: 0; transform: translateY(28px); } to { opacity: 1; transform: translateY(0); } }

@media (max-width: 560px) {
  .top-grid { grid-template-columns: minmax(0, 1fr); }
  .top-grid .status-card-wide, .mode-toggle-card { min-height: 142px; }

  /* 手机窄屏：状态文字允许收缩，但 BLE 按钮始终保留完整单行宽度 */
  .card-bottom-bar {
    gap: 10px;
    align-items: center;
  }
  .status-main {
    flex: 1 1 0;
    min-width: 0;
  }
  .sub-title {
    display: block;
    line-height: 1.55;
    word-break: break-word;
  }
  .ble-scan-btn {
    width: 100px;
    min-width: 100px;
    flex-basis: 100px;
    padding: 0 10px;
  }

  .mode-toggle-card { border-radius: 28px 12px 28px 12px; }
  .face-title { font-size: 18px; }
  .metric-topline { gap: 9px; }
  .metric-number { font-size: 25px; }
  .info-time { font-size: 27px; }
  .emotion-section { margin-left: 0; }
  .tracking-section { margin-right: 0; }
}

/* =========================================
   🌘 黑夜模式：GitHub Dim / Blue Slate
   ========================================= */
.dark-theme .night-bg { background: linear-gradient(135deg, #0D1117, #161B22); }
.dark-theme .splash-screen { background-color: #0D1117; }
.dark-theme .aura-glow { background: radial-gradient(ellipse, rgba(88, 166, 255, 0.22), transparent 70%); }
.dark-theme .pear-shape-outline { border-color: rgba(88, 166, 255, 0.78); box-shadow: 0 0 25px rgba(88, 166, 255, 0.34), inset 0 0 30px rgba(31, 111, 235, 0.32); }
.dark-theme .glass-pupil { background: radial-gradient(circle at 50% 20%, #0D1117 10%, #1F6FEB 58%, #58A6FF 100%); box-shadow: inset 0 -10px 20px rgba(201, 209, 217, 0.42), inset 0 10px 15px rgba(0, 0, 0, 0.72), 0 0 22px rgba(88, 166, 255, 0.32); }
.dark-theme .tail-left { background: linear-gradient(270deg, rgba(88, 166, 255, 0.62), transparent); box-shadow: -5px 0 10px rgba(88, 166, 255, 0.28); }
.dark-theme .tail-right { background: linear-gradient(90deg, rgba(88, 166, 255, 0.62), transparent); box-shadow: 5px 0 10px rgba(88, 166, 255, 0.28); }
.dark-theme .dot-online { background-color: #3FB950; box-shadow: 0 0 8px rgba(63, 185, 80, 0.42); }
.dark-theme .dot-offline { background-color: #F85149; box-shadow: 0 0 8px rgba(248, 81, 73, 0.34); }
.dark-theme .dot-pending { background: #D29922; box-shadow: 0 0 8px rgba(210, 153, 34, 0.36); }
.dark-theme .ble-scan-btn { background: linear-gradient(135deg, #21262D, #58A6FF); box-shadow: 0 5px 16px rgba(88, 166, 255, 0.24); }
.dark-theme .mode-icon-scene { background: linear-gradient(145deg, rgba(88, 166, 255, 0.18), rgba(22, 27, 34, 0.9)); border-color: rgba(88, 166, 255, 0.38); box-shadow: 0 8px 20px var(--shadow-color), inset 0 0 16px rgba(88, 166, 255, 0.08); }
.dark-theme .mode-icon-face { color: var(--accent-color); text-shadow: 0 0 10px rgba(88, 166, 255, 0.32); }
.dark-theme .mode-icon-orbit { border-color: rgba(88, 166, 255, 0.46); }
.dark-theme .action-icon-btn:active .icon-wrapper { background: rgba(88, 166, 255, 0.13); border-color: rgba(88, 166, 255, 0.58); box-shadow: 0 0 17px rgba(88, 166, 255, 0.24), inset 0 0 10px rgba(88, 166, 255, 0.13); }
.dark-theme .action-icon-btn:active .btn-label, .dark-theme .action-icon-btn.btn-active .btn-label { color: var(--accent-color); text-shadow: 0 0 8px rgba(88, 166, 255, 0.32); }
.dark-theme .action-icon-btn.btn-active .icon-wrapper { background: rgba(88, 166, 255, 0.16); border-color: rgba(88, 166, 255, 0.68); box-shadow: 0 0 20px rgba(88, 166, 255, 0.28), inset 0 0 12px rgba(88, 166, 255, 0.16); }
.dark-theme .chip-btn:active { background: rgba(88, 166, 255, 0.14); border-color: var(--accent-color); color: var(--accent-color); box-shadow: 0 0 12px rgba(88, 166, 255, 0.22); }
.dark-theme .drawer-primary-btn { background: linear-gradient(135deg, #21262D, #2381D9); box-shadow: 0 6px 18px rgba(88, 166, 255, 0.24); }
.dark-theme .wifi-drawer { background: #0D1117; }
.dark-theme .face-section, .dark-theme .info-metric { background: rgba(22, 27, 34, 0.78); }
.dark-theme .drawer-input, .dark-theme .drawer-secondary-btn { background: #161B22; }
.dark-theme .droplet-mark:before { background: rgba(88, 166, 255, 0.14); border-color: #58A6FF; }
.dark-theme .droplet-mark:after { background: #58A6FF; box-shadow: 0 0 8px rgba(88, 166, 255, 0.34); }
.dark-theme .humidity-fill { background: linear-gradient(90deg, #2F81F7, #58A6FF); }
.dark-theme .blink-core { background: linear-gradient(145deg, #C9D1D9 0%, #58A6FF 56%, #1F6FEB 100%); box-shadow: 4px 6px 0 rgba(1, 4, 9, 0.42), inset 2px 2px 3px rgba(255, 255, 255, 0.38); }
.dark-theme .blink-core:before { background: #0D1117; }
.dark-theme .blink-core:after { background: #79C0FF; }
.dark-theme .blink-spark { background: #58A6FF; box-shadow: 1px 2px 0 rgba(13, 17, 23, 0.5); }
.dark-theme .mask-back { background: linear-gradient(145deg, #C9D1D9, #6E7681); box-shadow: 3px 4px 0 rgba(1, 4, 9, 0.42), inset 2px 2px 3px rgba(255, 255, 255, 0.25); }
.dark-theme .mask-front { background: linear-gradient(145deg, #79C0FF, #1F6FEB); box-shadow: 3px 4px 0 rgba(1, 4, 9, 0.44), inset 2px 2px 3px rgba(255, 255, 255, 0.28); }
.dark-theme .mask-face:before { background: #0D1117; box-shadow: 8px 0 0 #0D1117; }
.dark-theme .mask-face:after { border-bottom-color: rgba(13, 17, 23, 0.82); }
.dark-theme .icon-wander .wander-orbit { border-color: #8B949E; box-shadow: 0 3px 0 rgba(1, 4, 9, 0.42); }
.dark-theme .icon-wander .wander-core { background: radial-gradient(circle at 32% 28%, #C9D1D9, #58A6FF 48%, #1F6FEB 100%); box-shadow: 3px 5px 0 rgba(1, 4, 9, 0.44), inset 2px 2px 3px rgba(255, 255, 255, 0.28); }
.dark-theme .icon-wander .wander-arrow { border-left-color: #58A6FF; filter: drop-shadow(1px 2px 0 rgba(1, 4, 9, 0.48)); }

/* =========================================
   🧊 3D 功能图标：灵瞬 / 幻相 / 游荡
   ========================================= */
.icon-3d { width: 38px; height: 38px; position: relative; transform-style: preserve-3d; transition: transform 0.28s ease; }
.action-icon-btn:active .icon-3d { transform: translateY(2px) scale(0.9); }
.blink-core { position: absolute; left: 6px; top: 12px; width: 27px; height: 15px; border-radius: 52% 48% 46% 54%; background: linear-gradient(145deg, #f2d39e 0%, #d39a61 54%, #9d6248 100%); transform: rotateX(25deg) rotateY(-16deg) translateZ(4px); box-shadow: 4px 6px 0 rgba(77, 48, 39, 0.22), inset 2px 2px 3px rgba(255, 247, 222, 0.55); }
.blink-core:before { content: ''; position: absolute; left: 7px; top: 6px; width: 13px; height: 3px; border-radius: 50%; background: #573d35; transform: rotate(-4deg); }
.blink-core:after { content: ''; position: absolute; right: -3px; bottom: -4px; width: 7px; height: 7px; border-radius: 50%; background: #c98354; box-shadow: inset 1px 1px 2px rgba(255,255,255,0.35); }
.blink-spark { position: absolute; width: 6px; height: 6px; background: #e2b16d; transform: rotate(45deg); border-radius: 2px; box-shadow: 1px 2px 0 rgba(89, 57, 40, 0.18); }
.spark-a { left: 3px; top: 4px; }
.spark-b { right: 2px; top: 1px; width: 4px; height: 4px; opacity: 0.72; }
.icon-illusion .mask-face { position: absolute; width: 23px; height: 17px; border-radius: 52% 48% 46% 54%; transform-style: preserve-3d; }
.mask-back { left: 10px; top: 5px; background: linear-gradient(145deg, #9aaeb0, #61777b); transform: rotate(13deg) translateZ(1px); box-shadow: 3px 4px 0 rgba(45, 50, 50, 0.27), inset 2px 2px 3px rgba(238, 247, 239, 0.32); }
.mask-front { left: 3px; top: 14px; background: linear-gradient(145deg, #e0b27e, #a9674c); transform: rotate(-12deg) translateZ(7px); box-shadow: 3px 4px 0 rgba(72, 43, 39, 0.26), inset 2px 2px 3px rgba(255, 239, 205, 0.4); }
.mask-face:before { content: ''; position: absolute; left: 6px; top: 6px; width: 4px; height: 3px; border-radius: 50%; background: #4f4140; box-shadow: 8px 0 0 #4f4140; }
.mask-face:after { content: ''; position: absolute; left: 8px; bottom: 3px; width: 8px; height: 3px; border-bottom: 1px solid rgba(78, 55, 48, 0.8); border-radius: 50%; }
.icon-wander .wander-orbit { position: absolute; left: 2px; top: 10px; width: 34px; height: 17px; border: 2px solid #9caa9e; border-radius: 50%; transform: rotateX(62deg) rotateZ(-22deg); opacity: 0.86; box-shadow: 0 3px 0 rgba(50, 65, 57, 0.22); }
.icon-wander .wander-core { position: absolute; left: 12px; top: 8px; width: 15px; height: 15px; border-radius: 50%; background: radial-gradient(circle at 32% 28%, #d9e0c8, #8eaa8d 48%, #58725f 100%); transform: translateZ(6px); box-shadow: 3px 5px 0 rgba(42, 54, 46, 0.24), inset 2px 2px 3px rgba(255,255,255,0.35); }
.icon-wander .wander-arrow { position: absolute; right: 1px; top: 3px; width: 0; height: 0; border-top: 5px solid transparent; border-bottom: 5px solid transparent; border-left: 8px solid #c8a26a; transform: rotate(-24deg) translateZ(8px); filter: drop-shadow(1px 2px 0 rgba(57, 45, 35, 0.2)); }
.action-icon-btn.btn-active .icon-illusion .mask-front { animation: mask-float 1.8s ease-in-out infinite alternate; }
.action-icon-btn.btn-active .icon-wander .wander-orbit { animation: wander-spin 2.8s linear infinite; }
.blink-btn:active .blink-core { transform: rotateX(44deg) rotateY(-16deg) scaleY(0.48) translateZ(4px); }
@keyframes mask-float { from { transform: rotate(-12deg) translate3d(0, 0, 7px); } to { transform: rotate(-7deg) translate3d(2px, -2px, 9px); } }
@keyframes wander-spin { from { transform: rotateX(62deg) rotateZ(-22deg); } to { transform: rotateX(62deg) rotateZ(338deg); } }

/* 顶部 WiFi 配置入口：恢复简洁胶囊样式，交互集中在配置抽屉 */
.wifi-pill { color: var(--text-sub); background: var(--btn-bg); overflow: visible; transition: transform 0.22s ease, border-color 0.22s ease, color 0.22s ease; }
.wifi-pill:active { transform: translateY(1px) scale(0.98); border-color: var(--border-color); }
.wifi-signal-icon { width: 17px; height: 14px; position: relative; color: currentColor; flex-shrink: 0; }
.wifi-signal-icon:before, .wifi-signal-icon:after { content: ''; position: absolute; left: 50%; border: 1.5px solid currentColor; border-bottom: 0; border-radius: 50% 50% 0 0; transform: translateX(-50%); }
.wifi-signal-icon:before { width: 16px; height: 10px; top: 0; opacity: 0.45; }
.wifi-signal-icon:after { width: 9px; height: 6px; top: 4px; opacity: 0.75; }
.wifi-signal-icon view { position: absolute; left: 50%; bottom: 0; width: 3px; height: 3px; border-radius: 50%; background: currentColor; transform: translateX(-50%); }
.wifi-signal-icon view:nth-child(2), .wifi-signal-icon view:nth-child(3) { display: none; }

/* 配网抽屉按钮和连接阶段 */
.drawer-status-row { transition: color 0.3s ease; }
.drawer-status-sending, .drawer-status-waiting { color: var(--accent-color); }
.drawer-status-success { color: #3fb950; }
.drawer-status-error { color: #f85149; }
.drawer-primary-btn { position: relative; overflow: hidden; display: flex; align-items: center; justify-content: center; gap: 8px; transition: transform 0.25s ease, filter 0.25s ease, background 0.3s ease, box-shadow 0.3s ease; }
.drawer-primary-btn:before { content: ''; position: absolute; inset: 0; background: linear-gradient(112deg, transparent 22%, rgba(255,255,255,0.28) 49%, transparent 74%); transform: translateX(-135%); transition: transform 0.65s ease; pointer-events: none; }
.drawer-primary-btn:not([disabled]):hover:before, .drawer-primary-btn:not([disabled]):active:before { transform: translateX(135%); }
.drawer-primary-btn > text, .drawer-primary-btn > view { position: relative; z-index: 1; }
.drawer-primary-btn:not([disabled]):active { transform: translateY(2px) scale(0.985); filter: brightness(0.94); }
.drawer-primary-btn.is-loading { animation: config-button-pulse 1.15s ease-in-out infinite; }
.drawer-primary-btn.is-success { background: linear-gradient(135deg, #238636, #3fb950); box-shadow: 0 7px 20px rgba(63, 185, 80, 0.22); }
.drawer-primary-btn.is-error { background: linear-gradient(135deg, #8e3b36, #f85149); box-shadow: 0 7px 20px rgba(248, 81, 73, 0.18); }
.button-spinner { width: 13px; height: 13px; border: 2px solid rgba(255,255,255,0.38); border-top-color: #fff; border-radius: 50%; animation: button-spin 0.75s linear infinite; }
@keyframes button-spin { to { transform: rotate(360deg); } }
@keyframes config-button-pulse { 0%, 100% { box-shadow: 0 6px 18px rgba(88, 166, 255, 0.2); } 50% { box-shadow: 0 8px 26px rgba(88, 166, 255, 0.42); } }
.wifi-flow { display: flex; align-items: center; justify-content: center; margin-top: 19px; color: var(--text-sub); }
.wifi-flow-step { display: flex; flex-direction: column; align-items: center; gap: 5px; font-size: 9px; letter-spacing: 0.7px; transition: color 0.3s ease; }
.wifi-flow-step.is-done { color: var(--accent-color); }
.flow-dot { width: 7px; height: 7px; border-radius: 50%; border: 1px solid var(--border-color); background: transparent; transition: all 0.3s ease; }
.wifi-flow-step.is-done .flow-dot { background: var(--accent-color); border-color: var(--accent-color); box-shadow: 0 0 8px rgba(88, 166, 255, 0.35); }
.flow-line { width: 54px; height: 1px; margin: -12px 8px 0; background: var(--border-color); }
</style>
