# ESP32S3 + Jetson Tegrastats 完整部署指南

## 系统架构
```
[Jetson Orin] ← tegrastats → [Python API Server] ← HTTP/WebSocket → [ESP32S3]
    (硬件)         (系统工具)      (10.10.99.98:5000)           (客户端设备)
```

## 1. Jetson端部署

### 1.1 环境准备
```bash
# 创建conda环境
conda create -n tegrastats-api python=3.9
conda activate tegrastats-api

# 安装依赖
pip install -r requirements.txt
```

### 1.2 服务启动
```bash
# 启动API服务
chmod +x start.sh
./start.sh

# 验证服务运行
curl http://10.10.99.98:5000/api/health
```

### 1.3 开机自启动设置
```bash
# 创建systemd服务文件
sudo nano /etc/systemd/system/tegrastats-api.service
```

服务文件内容：
```ini
[Unit]
Description=Tegrastats HTTP API Service
After=network.target

[Service]
Type=simple
User=your_username
WorkingDirectory=/path/to/rm01OrinStatus
ExecStart=/home/your_username/miniconda3/envs/tegrastats-api/bin/python app.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

启用服务：
```bash
sudo systemctl daemon-reload
sudo systemctl enable tegrastats-api.service
sudo systemctl start tegrastats-api.service
```

## 2. ESP32S3端部署

### 2.1 硬件准备
- ESP32S3开发板 (推荐ESP32-S3-DevKitC-1)
- USB-C数据线
- 稳定的5V电源(可选，用于长期运行)

### 2.2 开发环境配置

#### Arduino IDE 设置
1. 安装ESP32开发板支持包
   - 文件 → 首选项 → 附加开发板管理器网址
   - 添加：`https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - 工具 → 开发板 → 开发板管理器
   - 搜索并安装 "esp32" by Espressif Systems

2. 安装必需库
   - 工具 → 管理库
   - 搜索并安装：
     - `ArduinoJson` by Benoit Blanchon (6.21.3+)
     - `WebSockets` by Markus Sattler (2.3.6+) [仅WebSocket版本]

3. 板卡配置
   - 工具 → 开发板 → ESP32 Arduino → ESP32S3 Dev Module
   - 工具 → CPU频率 → 240MHz
   - 工具 → Flash大小 → 4MB (32Mb)
   - 工具 → PSRAM → Enabled

### 2.3 代码部署

#### REST API版本 (`esp32s3_example.cpp`)
```cpp
// 修改WiFi配置
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// 修改Jetson设备IP (如果不同)
const char* jetson_ip = "10.10.99.98";
```

#### WebSocket版本 (`esp32s3_websocket_example.cpp`)
```cpp
// 同样修改WiFi和IP配置
const char* ssid = "你的WiFi名称"; 
const char* password = "你的WiFi密码";
const char* jetson_ip = "10.10.99.98";
```

### 2.4 编译和上传
1. 选择正确的串口 (工具 → 端口)
2. 点击上传按钮
3. 打开串口监视器 (波特率115200)
4. 观察连接和数据接收状态

## 3. 网络配置

### 3.1 网络拓扑要求
- Jetson设备IP：10.10.99.98
- ESP32S3：自动获取IP (DHCP)
- 两设备需在同一网络中或能互相访问

### 3.2 防火墙配置 (Jetson端)
```bash
# 允许5000端口访问
sudo ufw allow 5000/tcp

# 或者完全关闭防火墙 (不推荐生产环境)
sudo ufw disable
```

### 3.3 网络测试
```bash
# 从其他设备测试API可访问性
curl http://10.10.99.98:5000/api/health
ping 10.10.99.98
```

## 4. 监控和维护

### 4.1 日志监控
```bash
# 查看服务日志
sudo journalctl -u tegrastats-api.service -f

# 查看最近错误
sudo journalctl -u tegrastats-api.service --since today
```

### 4.2 性能监控
```bash
# 检查服务状态
sudo systemctl status tegrastats-api.service

# 检查端口占用
sudo netstat -tlnp | grep :5000

# 检查内存使用
ps aux | grep python
```

### 4.3 ESP32S3监控
通过串口监视器观察：
- WiFi连接状态
- 数据接收频率
- 内存使用情况
- 连接质量评分

## 5. 故障排除

### 5.1 常见问题

#### Jetson端问题
| 问题 | 症状 | 解决方案 |
|------|------|----------|
| 服务启动失败 | 5000端口无响应 | 检查依赖安装，查看日志 |
| tegrastats权限错误 | 解析失败 | 添加用户到适当用户组 |
| 高CPU使用率 | 系统卡顿 | 调整更新频率，优化解析 |

#### ESP32S3端问题  
| 问题 | 症状 | 解决方案 |
|------|------|----------|
| WiFi连接失败 | 无法获取IP | 检查SSID/密码，信号强度 |
| JSON解析失败 | 数据显示异常 | 增加缓冲区大小 |
| 频繁重连 | 连接不稳定 | 检查网络质量，电源稳定性 |
| 内存不足 | 重启或崩溃 | 启用PSRAM，优化代码 |

### 5.2 调试技巧

#### API服务调试
```bash
# 手动测试API端点
curl -s http://10.10.99.98:5000/api/status | jq '.'
curl -s http://10.10.99.98.5000/api/cpu | jq '.'

# WebSocket连接测试
wscat -c ws://10.10.99.98:5000/socket.io/?EIO=4&transport=websocket
```

#### ESP32S3调试
```cpp
// 增加调试输出
#define DEBUG_MODE 1
#if DEBUG_MODE
  Serial.println("调试信息: " + debug_info);
#endif

// 内存监控
void printMemoryUsage() {
  Serial.printf("可用堆: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("可用PSRAM: %d bytes\n", ESP.getFreePsram());
}
```

## 6. 性能优化

### 6.1 Jetson端优化
```python
# config.py 优化配置
UPDATE_INTERVAL = 1.0  # 根据需要调整
MAX_CONNECTIONS = 5    # 限制连接数
BUFFER_SIZE = 4096     # 调整缓冲区大小
```

### 6.2 ESP32S3端优化
```cpp
// 减少内存使用
DynamicJsonDocument doc(4096); // 根据实际需要调整

// 降低更新频率（如果不需要1Hz）
const unsigned long update_interval = 2000; // 2秒

// 启用深度睡眠（适合电池供电）
esp_sleep_enable_timer_wakeup(1000000); // 1秒后唤醒
```

## 7. 扩展功能

### 7.1 数据存储
```cpp
// ESP32S3 SD卡存储
#include <SD.h>
void logDataToSD(const SystemStatus& status) {
  File dataFile = SD.open("/data.csv", FILE_APPEND);
  if (dataFile) {
    dataFile.printf("%lu,%.1f,%.1f,%.1f\n", 
                    status.timestamp,
                    status.cpu_avg_usage,
                    status.memory_usage_percent,
                    status.cpu_temperature);
    dataFile.close();
  }
}
```

### 7.2 MQTT发布
```cpp
// 发布到MQTT服务器
#include <PubSubClient.h>
void publishToMQTT(const SystemStatus& status) {
  StaticJsonDocument<512> doc;
  doc["cpu"] = status.cpu_avg_usage;
  doc["memory"] = status.memory_usage_percent;
  doc["temperature"] = status.cpu_temperature;
  
  String payload;
  serializeJson(doc, payload);
  mqtt.publish("jetson/status", payload.c_str());
}
```

### 7.3 Web界面
```cpp
// ESP32S3内置Web服务器
#include <WebServer.h>
WebServer server(80);

void handleRoot() {
  String html = generateStatusHTML(current_status);
  server.send(200, "text/html", html);
}
```

## 8. 安全考虑

### 8.1 网络安全
- 使用防火墙限制访问端口
- 考虑使用VPN进行远程访问
- 定期更新系统和依赖包

### 8.2 设备安全  
- 避免在公共WiFi上使用
- 使用强密码保护WiFi网络
- 考虑使用加密通信

### 8.3 数据保护
- 不记录敏感信息到日志
- 定期清理临时文件
- 备份重要配置文件

这个完整的部署指南涵盖了从硬件准备到系统维护的所有方面，帮助你成功部署ESP32S3 + Jetson tegrastats监控系统。