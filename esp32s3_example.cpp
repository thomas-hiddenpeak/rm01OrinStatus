/*
 * ESP32S3 Tegrastats API Client 示例
 * 
 * 此示例演示如何从ESP32S3访问Jetson设备的tegrastats HTTP API
 * 支持REST API轮询和数据解析显示
 * 
 * 硬件要求:
 * - ESP32S3开发板
 * - WiFi连接
 * 
 * 库依赖:
 * - WiFi (ESP32 Core)
 * - HTTPClient (ESP32 Core)  
 * - ArduinoJson (6.x版本)
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Jetson设备API配置
const char* jetson_ip = "10.10.99.98";
const int jetson_port = 5000;
const char* api_endpoint = "/api/status";

// 更新间隔（毫秒）
const unsigned long update_interval = 1000; // 1秒

// 全局变量
unsigned long last_update = 0;
bool wifi_connected = false;

// 系统状态结构体
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
};

SystemStatus current_status = {0};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32S3 Jetson Monitor 启动 ===");
  Serial.println("版本: 1.0");
  Serial.println("目标设备: " + String(jetson_ip) + ":" + String(jetson_port));
  
  // 初始化WiFi
  connectToWiFi();
  
  // 测试API连接
  if (wifi_connected) {
    testAPIConnection();
  }
  
  Serial.println("=== 开始监控 ===\n");
}

void loop() {
  // 检查WiFi连接
  checkWiFiConnection();
  
  // 定期获取数据
  if (wifi_connected && (millis() - last_update >= update_interval)) {
    fetchSystemStatus();
    last_update = millis();
  }
  
  delay(100); // 避免过度占用CPU
}

void connectToWiFi() {
  Serial.print("连接WiFi: " + String(ssid) + " ");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_connected = true;
    Serial.println("\n✅ WiFi连接成功!");
    Serial.println("IP地址: " + WiFi.localIP().toString());
    Serial.println("信号强度: " + String(WiFi.RSSI()) + " dBm");
  } else {
    wifi_connected = false;
    Serial.println("\n❌ WiFi连接失败!");
  }
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    if (wifi_connected) {
      Serial.println("⚠️ WiFi连接丢失，尝试重连...");
      wifi_connected = false;
    }
    connectToWiFi();
  }
}

void testAPIConnection() {
  Serial.println("测试API连接...");
  HTTPClient http;
  String url = "http://" + String(jetson_ip) + ":" + String(jetson_port) + "/api/health";
  
  http.begin(url);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  if (httpCode == 200) {
    String response = http.getString();
    Serial.println("✅ API连接测试成功!");
    Serial.println("服务器响应: " + response);
  } else {
    Serial.println("❌ API连接测试失败! HTTP代码: " + String(httpCode));
  }
  
  http.end();
}

void fetchSystemStatus() {
  HTTPClient http;
  String url = "http://" + String(jetson_ip) + ":" + String(jetson_port) + api_endpoint;
  
  http.begin(url);
  http.setTimeout(5000);
  
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    String payload = http.getString();
    
    // 解析JSON数据
    DynamicJsonDocument doc(8192); // 8KB缓冲区
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.println("❌ JSON解析失败: " + String(error.c_str()));
      current_status.data_valid = false;
    } else {
      parseSystemData(doc);
      displaySystemStatus();
    }
  } else {
    Serial.println("❌ HTTP请求失败: " + String(httpCode));
    current_status.data_valid = false;
  }
  
  http.end();
}

void parseSystemData(const DynamicJsonDocument& doc) {
  // 解析CPU数据
  JsonArray cores = doc["cpu"]["cores"];
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
  JsonObject ram = doc["memory"]["ram"];
  current_status.memory_used_gb = ram["used"].as<float>() / 1024.0;
  current_status.memory_total_gb = ram["total"].as<float>() / 1024.0;
  current_status.memory_usage_percent = (current_status.memory_used_gb / current_status.memory_total_gb) * 100;
  
  // 解析温度数据
  current_status.cpu_temperature = doc["temperature"]["cpu"];
  
  // 解析功耗数据
  float cpu_power = doc["power"]["cpu_cv"]["current"];
  float gpu_power = doc["power"]["gpu_soc"]["current"];
  float sys_power = doc["power"]["sys_5v"]["current"];
  current_status.total_power_watts = (cpu_power + gpu_power + sys_power) / 1000.0;
  
  // 记录时间戳
  current_status.timestamp = millis();
  current_status.data_valid = true;
}

void displaySystemStatus() {
  // 清屏（可选）
  // Serial.write(27); Serial.print("[2J"); Serial.write(27); Serial.print("[H");
  
  Serial.println("📊 === Jetson 系统状态 ===");
  Serial.println("⏰ 更新时间: " + String(millis() / 1000) + "s");
  
  if (current_status.data_valid) {
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
    Serial.printf("🌡️  温度: %.1f°C", current_status.cpu_temperature);
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
    
    // 系统健康评估
    displaySystemHealth();
    
  } else {
    Serial.println("❌ 数据无效或获取失败");
  }
  
  Serial.println("========================\n");
}

void displaySystemHealth() {
  Serial.print("🏥 系统健康: ");
  
  int health_score = 100;
  String health_issues = "";
  
  // CPU负载检查
  if (current_status.cpu_avg_usage > 80) {
    health_score -= 20;
    health_issues += "CPU高负载 ";
  }
  
  // 内存使用检查
  if (current_status.memory_usage_percent > 90) {
    health_score -= 25;
    health_issues += "内存不足 ";
  } else if (current_status.memory_usage_percent > 80) {
    health_score -= 10;
    health_issues += "内存使用较高 ";
  }
  
  // 温度检查
  if (current_status.cpu_temperature > 85) {
    health_score -= 30;
    health_issues += "过热风险 ";
  } else if (current_status.cpu_temperature > 75) {
    health_score -= 15;
    health_issues += "温度偏高 ";
  }
  
  // 功耗检查
  if (current_status.total_power_watts > 20) {
    health_score -= 10;
    health_issues += "功耗过高 ";
  }
  
  // 输出健康状态
  if (health_score >= 90) {
    Serial.println("优秀 ✅ (" + String(health_score) + "/100)");
  } else if (health_score >= 70) {
    Serial.println("良好 🟡 (" + String(health_score) + "/100) - " + health_issues);
  } else if (health_score >= 50) {
    Serial.println("注意 🟠 (" + String(health_score) + "/100) - " + health_issues);
  } else {
    Serial.println("警告 🔴 (" + String(health_score) + "/100) - " + health_issues);
  }
}

// 获取特定API端点数据的辅助函数
String fetchSpecificData(const char* endpoint) {
  HTTPClient http;
  String url = "http://" + String(jetson_ip) + ":" + String(jetson_port) + endpoint;
  
  http.begin(url);
  http.setTimeout(3000);
  
  int httpCode = http.GET();
  String result = "";
  
  if (httpCode == 200) {
    result = http.getString();
  }
  
  http.end();
  return result;
}

// 内存使用情况监控
void printMemoryUsage() {
  Serial.println("ESP32S3 内存使用情况:");
  Serial.printf("空闲堆内存: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("最小空闲堆: %d bytes\n", ESP.getMinFreeHeap());
  Serial.printf("堆大小: %d bytes\n", ESP.getHeapSize());
  Serial.printf("PSRAM大小: %d bytes\n", ESP.getPsramSize());
  Serial.printf("空闲PSRAM: %d bytes\n", ESP.getFreePsram());
}