/*
 * ESP32S3 Tegrastats WebSocket Client 示例
 * 
 * 此示例演示如何通过WebSocket实时接收Jetson设备的tegrastats数据
 * 提供更高效的实时数据流和更低的延迟
 * 
 * 硬件要求:
 * - ESP32S3开发板
 * - WiFi连接
 * 
 * 库依赖:
 * - WiFi (ESP32 Core)
 * - WebSocketsClient (by Markus Sattler)
 * - ArduinoJson (6.x版本)
 * 
 * 安装库：
 * 1. 打开Arduino IDE库管理器
 * 2. 搜索并安装 "WebSockets" by Markus Sattler
 * 3. 搜索并安装 "ArduinoJson" by Benoit Blanchon
 */

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Jetson设备WebSocket配置
const char* jetson_ip = "10.10.99.98";
const int jetson_port = 5000;
const char* websocket_path = "/socket.io/?EIO=4&transport=websocket";

// 连接状态
bool wifi_connected = false;
bool websocket_connected = false;
unsigned long last_heartbeat = 0;
unsigned long last_data_received = 0;
unsigned long connection_start_time = 0;
int data_packet_count = 0;

// WebSocket客户端
WebSocketsClient webSocket;

// 系统状态结构体（与REST版本相同）
struct SystemStatus {
  float cpu_avg_usage;
  float memory_used_gb;
  float memory_total_gb;
  float memory_usage_percent;
  float cpu_temperature;
  float total_power_watts;
  int active_cpu_cores;
  unsigned long timestamp;
  bool data_valid;
  float update_frequency; // 实际更新频率
};

SystemStatus current_status = {0};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32S3 Jetson WebSocket Monitor 启动 ===");
  Serial.println("版本: 2.0 (WebSocket版本)");
  Serial.println("目标设备: " + String(jetson_ip) + ":" + String(jetson_port));
  
  // 初始化WiFi
  connectToWiFi();
  
  // 初始化WebSocket连接
  if (wifi_connected) {
    initWebSocket();
  }
  
  Serial.println("=== 开始实时监控 ===\n");
}

void loop() {
  // 检查WiFi连接
  checkWiFiConnection();
  
  // 处理WebSocket事件
  if (wifi_connected) {
    webSocket.loop();
    
    // 连接状态监控
    monitorConnectionHealth();
  }
  
  delay(10); // 短延迟以保持响应性
}

void connectToWiFi() {
  Serial.print("连接WiFi: " + String(ssid) + " ");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("✅ WiFi连接成功!");
    Serial.println("IP地址: " + WiFi.localIP().toString());
    Serial.println("信号强度: " + String(WiFi.RSSI()) + " dBm");
  } else {
    wifi_connected = false;
    Serial.println("❌ WiFi连接失败!");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    if (wifi_connected) {
      Serial.println("⚠️ WiFi连接丢失，尝试重连...");
      wifi_connected = false;
      websocket_connected = false;
      webSocket.disconnect();
    }
    connectToWiFi();
    if (wifi_connected) {
      initWebSocket();
    }
  }
}

void initWebSocket() {
  Serial.println("初始化WebSocket连接...");
  
  // 配置WebSocket客户端
  webSocket.begin(jetson_ip, jetson_port, "/socket.io/?EIO=4&transport=websocket");
  
  // 设置事件处理器
  webSocket.onEvent(webSocketEvent);
  
  // 配置重连间隔
  webSocket.setReconnectInterval(5000);
  
  // 启用心跳包
  webSocket.enableHeartbeat(25000, 3000, 2);
  
  connection_start_time = millis();
  data_packet_count = 0;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("🔌 WebSocket连接断开");
      websocket_connected = false;
      break;
      
    case WStype_CONNECTED:
      Serial.printf("✅ WebSocket连接成功: %s\n", payload);
      websocket_connected = true;
      last_heartbeat = millis();
      
      // 发送Socket.IO连接消息
      webSocket.sendTXT("40");
      break;
      
    case WStype_TEXT:
      handleWebSocketMessage((char*)payload, length);
      break;
      
    case WStype_BIN:
      Serial.printf("收到二进制数据，长度: %u\n", length);
      break;
      
    case WStype_ERROR:
      Serial.printf("❌ WebSocket错误: %s\n", payload);
      break;
      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      // 处理分片消息（如果需要）
      break;
      
    case WStype_PING:
      Serial.println("📡 收到Ping");
      break;
      
    case WStype_PONG:
      Serial.println("📡 收到Pong");
      last_heartbeat = millis();
      break;
  }
}

void handleWebSocketMessage(const char* message, size_t length) {
  // Socket.IO消息格式处理
  String msg = String(message);
  
  if (msg.startsWith("40")) {
    // 连接确认
    Serial.println("📡 Socket.IO连接确认");
    return;
  }
  
  if (msg.startsWith("42")) {
    // 数据消息
    int jsonStart = msg.indexOf('[');
    if (jsonStart != -1) {
      String jsonData = msg.substring(jsonStart);
      handleDataMessage(jsonData);
    }
    return;
  }
  
  if (msg == "2") {
    // 心跳ping
    webSocket.sendTXT("3"); // 发送pong
    last_heartbeat = millis();
    return;
  }
  
  // 其他消息类型
  Serial.println("收到未处理消息: " + msg);
}

void handleDataMessage(const String& jsonData) {
  // 解析WebSocket数据消息
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, jsonData);
  
  if (error) {
    Serial.println("❌ WebSocket数据解析失败: " + String(error.c_str()));
    return;
  }
  
  // 检查是否是tegrastats数据
  if (doc.is<JsonArray>() && doc.size() >= 2) {
    JsonArray msgArray = doc.as<JsonArray>();
    String eventName = msgArray[0];
    
    if (eventName == "tegrastats_update") {
      JsonObject data = msgArray[1];
      parseSystemData(data);
      displaySystemStatus();
      
      // 更新统计信息
      data_packet_count++;
      last_data_received = millis();
      
      // 计算实际更新频率
      if (connection_start_time > 0 && data_packet_count > 1) {
        unsigned long elapsed = millis() - connection_start_time;
        current_status.update_frequency = (data_packet_count * 1000.0) / elapsed;
      }
    }
  }
}

void parseSystemData(const JsonObject& data) {
  // 解析CPU数据
  JsonArray cores = data["cpu"]["cores"];
  float total_cpu_usage = 0;
  int active_cores = 0;
  
  for (JsonObject core : cores) {
    float usage = core["usage"];
    total_cpu_usage += usage;
    if (usage > 0) active_cores++;
  }
  current_status.cpu_avg_usage = total_cpu_usage / cores.size();
  current_status.active_cpu_cores = active_cores;
  
  // 解析内存数据
  JsonObject ram = data["memory"]["ram"];
  current_status.memory_used_gb = ram["used"].as<float>() / 1024.0;
  current_status.memory_total_gb = ram["total"].as<float>() / 1024.0;
  current_status.memory_usage_percent = (current_status.memory_used_gb / current_status.memory_total_gb) * 100;
  
  // 解析温度数据
  current_status.cpu_temperature = data["temperature"]["cpu"];
  
  // 解析功耗数据
  float cpu_power = data["power"]["cpu_cv"]["current"];
  float gpu_power = data["power"]["gpu_soc"]["current"];
  float sys_power = data["power"]["sys_5v"]["current"];
  current_status.total_power_watts = (cpu_power + gpu_power + sys_power) / 1000.0;
  
  // 记录时间戳
  current_status.timestamp = millis();
  current_status.data_valid = true;
}

void displaySystemStatus() {
  Serial.println("📊 === Jetson 实时状态 (WebSocket) ===");
  Serial.println("⏰ 更新时间: " + String(millis() / 1000) + "s");
  
  if (current_status.data_valid) {
    // 实时更新频率
    Serial.printf("📡 更新频率: %.2f Hz (目标: 1.0 Hz)\n", current_status.update_frequency);
    
    // CPU信息
    Serial.printf("🔧 CPU: %.1f%% (活跃核心: %d/12)\n", 
                  current_status.cpu_avg_usage, 
                  current_status.active_cpu_cores);
    
    // 内存信息
    Serial.printf("💾 内存: %.1fGB/%.1fGB (%.1f%%)\n",
                  current_status.memory_used_gb,
                  current_status.memory_total_gb,
                  current_status.memory_usage_percent);
    
    // 温度信息
    Serial.printf("🌡️ 温度: %.1f°C", current_status.cpu_temperature);
    if (current_status.cpu_temperature > 80) {
      Serial.print(" ⚠️ 高温警告!");
    } else if (current_status.cpu_temperature > 70) {
      Serial.print(" ⚡ 温度较高");
    }
    Serial.println();
    
    // 功耗信息
    Serial.printf("⚡ 功耗: %.1fW", current_status.total_power_watts);
    if (current_status.total_power_watts > 15) {
      Serial.print(" 🔥 高功耗");
    } else if (current_status.total_power_watts > 10) {
      Serial.print(" 📈 中等功耗");
    } else {
      Serial.print(" 🌱 低功耗");
    }
    Serial.println();
    
    // 连接质量评估
    displayConnectionQuality();
    
  } else {
    Serial.println("❌ WebSocket数据无效或未收到");
  }
  
  Serial.println("================================\n");
}

void displayConnectionQuality() {
  Serial.print("🌐 连接质量: ");
  
  unsigned long now = millis();
  int quality_score = 100;
  String quality_issues = "";
  
  // 频率稳定性检查
  if (current_status.update_frequency < 0.8) {
    quality_score -= 20;
    quality_issues += "更新频率低 ";
  } else if (current_status.update_frequency > 1.2) {
    quality_score -= 10;
    quality_issues += "更新频率不稳定 ";
  }
  
  // 心跳检查
  if (now - last_heartbeat > 30000) {
    quality_score -= 30;
    quality_issues += "心跳异常 ";
  }
  
  // 数据延迟检查
  if (now - last_data_received > 2000) {
    quality_score -= 25;
    quality_issues += "数据延迟 ";
  }
  
  // WiFi信号强度检查
  int rssi = WiFi.RSSI();
  if (rssi < -80) {
    quality_score -= 15;
    quality_issues += "WiFi信号弱 ";
  }
  
  // 输出连接质量
  if (quality_score >= 90) {
    Serial.println("优秀 ✅ (" + String(quality_score) + "/100)");
  } else if (quality_score >= 70) {
    Serial.println("良好 🟡 (" + String(quality_score) + "/100) - " + quality_issues);
  } else if (quality_score >= 50) {
    Serial.println("一般 🟠 (" + String(quality_score) + "/100) - " + quality_issues);
  } else {
    Serial.println("较差 🔴 (" + String(quality_score) + "/100) - " + quality_issues);
  }
}

void monitorConnectionHealth() {
  unsigned long now = millis();
  
  // 检查数据接收超时
  if (websocket_connected && last_data_received > 0 && (now - last_data_received > 5000)) {
    Serial.println("⚠️ 数据接收超时，尝试重连WebSocket...");
    webSocket.disconnect();
    delay(1000);
    initWebSocket();
  }
  
  // 定期输出连接统计信息（每30秒）
  static unsigned long last_stats = 0;
  if (now - last_stats > 30000) {
    printConnectionStats();
    last_stats = now;
  }
}

void printConnectionStats() {
  Serial.println("\n📈 === 连接统计信息 ===");
  Serial.printf("连接时长: %lu 秒\n", (millis() - connection_start_time) / 1000);
  Serial.printf("数据包总数: %d\n", data_packet_count);
  Serial.printf("平均更新频率: %.2f Hz\n", current_status.update_frequency);
  Serial.printf("WiFi信号: %d dBm\n", WiFi.RSSI());
  Serial.printf("ESP32内存: %d KB 可用\n", ESP.getFreeHeap() / 1024);
  Serial.println("========================\n");
}

// 手动发送测试消息
void sendTestMessage() {
  if (websocket_connected) {
    webSocket.sendTXT("42[\"test_message\",{\"from\":\"ESP32S3\"}]");
    Serial.println("📤 发送测试消息");
  }
}