# Tegrastats API

一个用于NVIDIA Jetson设备系统监控的HTTP API和WebSocket服务器，基于tegrastats工具提供实时系统状态信息。支持pip安装和命令行使用。

## ✨ 功能特性

- **REST API**: 6个HTTP端点提供完整的系统状态信息
- **WebSocket实时推送**: 1Hz频率实时推送系统数据  
- **系统监控**: CPU使用率、内存状态、温度、功耗等完整信息
- **连接管理**: 支持多客户端连接，可配置最大连接数
- **命令行工具**: 丰富的CLI命令用于服务器管理和测试
- **Python库**: 可作为Python包导入使用
- **配置灵活**: 支持环境变量和命令行参数配置
- **系统服务**: 支持systemd服务部署，开机自启动

## 🚀 快速开始

### 方式一：Python包安装 (推荐)

```bash
# 从源码安装
pip install .

# 开发模式安装
pip install -e .

# 使用安装脚本（自动测试）
./install_package.sh
```

安装后直接使用：
```bash
# 启动服务器
tegrastats-api run

# 自定义配置启动
tegrastats-api run --host 0.0.0.0 --port 8080 --debug

# 测试连接
tegrastats-api test

# 实时监控
tegrastats-api monitor
```

### 方式二：系统服务部署

使用传统的systemd服务方式：

```bash
# 运行系统服务安装脚本
./install.sh --host 0.0.0.0 --port 58090

# 服务管理
sudo systemctl status tegrastats-api
sudo systemctl restart tegrastats-api
```

### 方式三：手动运行

```bash
# 安装依赖
pip install -r requirements.txt

# 直接运行（不推荐用于生产环境）
python app.py
nohup python app.py > app.log 2>&1 &
```

#### 3. 验证服务
```bash
# 检查服务状态
curl http://10.10.99.98:5000/api/health

# 获取完整系统状态
curl http://10.10.99.98:5000/api/status
```

## 🔧 使用方法

### 命令行工具

```bash
# 启动服务器
tegrastats-api run

# 自定义配置启动
tegrastats-api run --host 0.0.0.0 --port 8080 --debug

# 测试API连接
tegrastats-api test --host localhost --port 58090

# WebSocket实时监控
tegrastats-api monitor --host localhost --port 58090 --duration 30

# 查看当前配置
tegrastats-api config

# 查看帮助
tegrastats-api --help
```

### Python库使用

```python
from tegrastats_api import TegrastatsServer, Config

# 基本使用
server = TegrastatsServer()
server.run()

# 自定义配置
config = Config(host='0.0.0.0', port=8080, debug=True)
server = TegrastatsServer(config)
server.run()

# 上下文管理器
with TegrastatsServer(config) as server:
    # 服务器自动启动和停止
    pass
```

### 传统系统服务管理

```bash
# 服务基本操作（如果使用install.sh安装）
./service_control.sh start      # 启动服务
./service_control.sh stop       # 停止服务  
./service_control.sh restart    # 重启服务
./service_control.sh status     # 查看状态

# 日志和监控
./service_control.sh logs       # 查看实时日志
./service_control.sh recent     # 查看最近日志
./service_control.sh test       # 测试API连接
```

### systemd原生命令

```bash
# 服务状态管理
sudo systemctl start tegrastats-api      # 启动服务
sudo systemctl stop tegrastats-api       # 停止服务
sudo systemctl restart tegrastats-api    # 重启服务
sudo systemctl status tegrastats-api     # 查看状态

# 开机自启动
sudo systemctl enable tegrastats-api     # 启用自启动
sudo systemctl disable tegrastats-api    # 禁用自启动

# 日志查看
sudo journalctl -u tegrastats-api -f     # 实时日志
sudo journalctl -u tegrastats-api -n 50  # 最近50条日志
```

### 服务卸载

完全移除服务和相关配置：

```bash
./uninstall.sh
```

## 📡 API文档

### REST API端点

| 端点 | 方法 | 描述 | 返回数据 |
|------|------|------|----------|
| `/api/health` | GET | 服务健康检查 | 服务状态和连接数 |
| `/api/status` | GET | **完整系统状态** | 所有tegrastats数据 |
| `/api/cpu` | GET | CPU信息 | CPU核心使用率和频率 |
| `/api/memory` | GET | 内存信息 | RAM和SWAP使用情况 |
| `/api/temperature` | GET | 温度信息 | 各传感器温度 |
| `/api/power` | GET | 功耗信息 | 各电源域功耗 |

### WebSocket实时推送

- **连接地址**: `ws://host:port/socket.io/`
- **推送事件**: `tegrastats_update`
- **推送频率**: 1Hz（每秒1次）
- **数据格式**: 与`/api/status`相同
- **连接限制**: 最大10个并发连接

### 配置选项

#### 环境变量
```bash
export TEGRASTATS_API_HOST=0.0.0.0
export TEGRASTATS_API_PORT=58090
export TEGRASTATS_API_DEBUG=false
export TEGRASTATS_API_LOG_LEVEL=INFO
export TEGRASTATS_API_MAX_CONNECTIONS=10
export TEGRASTATS_API_UPDATE_INTERVAL=1.0
```

#### 命令行参数
```bash
tegrastats-api run \
  --host 0.0.0.0 \
  --port 58090 \
  --debug \
  --log-level INFO \
  --max-connections 10 \
  --update-interval 1.0
```

## 📊 数据格式说明

### 完整数据结构
```json
{
  "timestamp": "2025-10-03T04:54:14.408614Z",
  "cpu": {
    "cores": [
      {"id": 0, "usage": 6, "freq": 729},
      {"id": 1, "usage": 0, "freq": 729},
      ...12个CPU核心
    ]
  },
  "memory": {
    "ram": {"used": 2358, "total": 62841, "unit": "MB"},
    "swap": {"used": 0, "total": 31421, "cached": 0, "unit": "MB"}
  },
  "temperature": {
    "cpu": 47.437,
    "soc0": 44.718,
    "soc1": 45.843,
    "soc2": 45.343,
    "tj": 47.437
  },
  "power": {
    "cpu_cv": {"current": 493, "average": 306, "unit": "mW"},
    "gpu_soc": {"current": 2467, "average": 2472, "unit": "mW"},
    "sys_5v": {"current": 3582, "average": 3411, "unit": "mW"}
  },
  "gpu": {
    "gr3d_freq": 0
  }
}
```

### 数据字段详情

#### CPU信息 (`cpu`)
- **cores**: 数组，包含12个CPU核心
  - `id`: 核心编号 (0-11)
  - `usage`: 使用率百分比 (0-100)
  - `freq`: 工作频率 (MHz)

#### 内存信息 (`memory`)
- **ram**: 系统主内存
  - `used`: 已使用内存 (MB)
  - `total`: 总内存容量 (MB)
  - `unit`: 单位 "MB"
- **swap**: 交换分区
  - `used`: 已使用交换空间 (MB)
  - `total`: 总交换空间 (MB)
  - `cached`: 缓存大小 (MB)
  - `unit`: 单位 "MB"

#### 温度信息 (`temperature`)
- `cpu`: CPU温度 (°C)
- `soc0`: SOC0温度 (°C)
- `soc1`: SOC1温度 (°C)
- `soc2`: SOC2温度 (°C)
- `tj`: 结温 (°C)

#### 功耗信息 (`power`)
- **cpu_cv**: CPU核心电压功耗
- **gpu_soc**: GPU和SOC功耗
- **sys_5v**: 系统5V功耗
  - `current`: 当前功耗 (mW)
  - `average`: 平均功耗 (mW)
  - `unit`: 单位 "mW"

#### GPU信息 (`gpu`)
- `gr3d_freq`: GPU 3D引擎使用率百分比 (0-100)

#### 时间戳 (`timestamp`)
- ISO 8601格式的时间戳，表示数据采集时间

## 🔌 ESP32S3集成指南

### 方式1: REST API轮询（推荐）

**优点**: 简单可靠，易于实现
**缺点**: 需要定期轮询

#### Arduino代码示例
```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";
const char* serverURL = "http://10.10.99.98:5000/api/status";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("连接WiFi中...");
  }
  Serial.println("WiFi连接成功!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String payload = http.getString();
      
      // 解析JSON数据
      DynamicJsonDocument doc(8192);
      deserializeJson(doc, payload);
      
      // 提取关键数据
      float cpuUsage = 0;
      JsonArray cores = doc["cpu"]["cores"];
      for (JsonObject core : cores) {
        cpuUsage += core["usage"].as<float>();
      }
      cpuUsage /= cores.size();
      
      float memoryUsage = (float)doc["memory"]["ram"]["used"] / doc["memory"]["ram"]["total"] * 100;
      float cpuTemp = doc["temperature"]["cpu"];
      float totalPower = doc["power"]["cpu_cv"]["current"].as<float>() + 
                        doc["power"]["gpu_soc"]["current"].as<float>() + 
                        doc["power"]["sys_5v"]["current"].as<float>();
      
      // 输出数据
      Serial.printf("CPU使用率: %.1f%%\n", cpuUsage);
      Serial.printf("内存使用率: %.1f%%\n", memoryUsage);
      Serial.printf("CPU温度: %.1f°C\n", cpuTemp);
      Serial.printf("总功耗: %.1fW\n", totalPower / 1000);
      Serial.println("--------------------");
    }
    
    http.end();
  }
  
  delay(1000); // 1秒更新一次
}
```

#### 专门API端点使用
```cpp
// 仅获取CPU信息
http.begin("http://10.10.99.98:5000/api/cpu");

// 仅获取温度信息
http.begin("http://10.10.99.98:5000/api/temperature");

// 仅获取功耗信息  
http.begin("http://10.10.99.98:5000/api/power");
```

### 方式2: WebSocket实时推送

**优点**: 实时性好，服务器主动推送
**缺点**: 实现相对复杂

#### WebSocket连接示例
```cpp
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      Serial.println("WebSocket连接成功!");
      break;
      
    case WStype_TEXT:
      Serial.printf("收到数据: %s\n", payload);
      // 解析接收到的JSON数据
      DynamicJsonDocument doc(8192);
      deserializeJson(doc, payload);
      // 处理数据...
      break;
      
    case WStype_DISCONNECTED:
      Serial.println("WebSocket连接断开");
      break;
  }
}

void setup() {
  // WiFi连接代码...
  
  webSocket.begin("10.10.99.98", 5000, "/socket.io/?EIO=4&transport=websocket");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();
}
```

### ESP32S3硬件建议

#### 内存配置
- **最小堆内存**: 8KB（用于JSON解析）
- **建议堆内存**: 16KB（用于缓存和处理）

#### 网络配置
```cpp
// 设置HTTP超时
http.setTimeout(5000);

// 设置连接超时
WiFi.setTimeout(10000);
```

#### 错误处理
```cpp
// HTTP错误处理
if (httpResponseCode != 200) {
  Serial.printf("HTTP错误: %d\n", httpResponseCode);
  // 实现重试逻辑
}

// WiFi断线重连
if (WiFi.status() != WL_CONNECTED) {
  WiFi.reconnect();
}
```

## ⚙️ 服务配置

### 服务器配置
- **服务地址**: `10.10.99.98:5000`
- **最大并发连接**: 10个
- **数据更新频率**: 1Hz（每秒1次）
- **超时设置**: 5秒
- **CORS支持**: 允许跨域访问

### 自定义配置
编辑 `config.py` 文件：
```python
# 服务配置
SERVER_HOST = "10.10.99.98"  # 更改服务器IP
SERVER_PORT = 5000           # 更改端口
UPDATE_INTERVAL = 1.0        # 更改更新频率（秒）
MAX_CONNECTIONS = 10         # 更改最大连接数
```

## 🧪 测试工具

### 1. WebSocket测试
```bash
python test_simple_ws.py        # 简单WebSocket测试
python test_websocket_debug.py  # 详细WebSocket测试
```

### 2. API对比测试
```bash
python compare_apis.py          # 对比REST和WebSocket数据一致性
```

### 3. 性能测试
```bash
# 测试API响应时间
for i in {1..10}; do
  time curl -s http://10.10.99.98:5000/api/status > /dev/null
done
```

## � 部署脚本详细说明

### install.sh - 自动安装脚本

自动化安装脚本提供完整的部署解决方案：

**功能特性：**
- ✅ 自动检测系统环境和依赖
- ✅ 安装或配置Miniconda环境
- ✅ 创建并配置Python虚拟环境
- ✅ 安装所有必需的Python包
- ✅ 创建systemd系统服务
- ✅ 配置开机自启动和故障重启
- ✅ 验证安装和API服务

**使用方法：**
```bash
# 给脚本执行权限（如果需要）
chmod +x install.sh

# 运行安装
./install.sh
```

**安装过程：**
1. 环境检查（操作系统、依赖工具）
2. Conda环境安装/配置
3. Python虚拟环境创建
4. 依赖包安装
5. systemd服务创建
6. 服务启动和验证
7. 管理脚本生成

### service_control.sh - 服务管理脚本

简化的服务管理界面，提供常用操作：

```bash
# 基本服务操作
./service_control.sh start      # 启动服务
./service_control.sh stop       # 停止服务
./service_control.sh restart    # 重启服务
./service_control.sh status     # 详细状态信息

# 日志和监控
./service_control.sh logs       # 实时日志输出
./service_control.sh recent     # 最近日志记录

# 配置管理
./service_control.sh enable     # 启用开机自启
./service_control.sh disable    # 禁用开机自启
./service_control.sh reload     # 重载服务配置

# 测试和诊断
./service_control.sh test       # API连接测试
./service_control.sh info       # 服务信息摘要
```

### uninstall.sh - 完全卸载脚本

安全地移除所有组件和配置：

**卸载内容：**
- 🗑️ 停止并删除systemd服务
- 🗑️ 移除服务配置文件
- 🗑️ 删除conda虚拟环境
- 🗑️ 清理临时文件和缓存
- 🗑️ 检查并终止残留进程
- 💾 可选备份配置文件

**使用方法：**
```bash
./uninstall.sh
```

### 系统服务特性

使用systemd管理的服务具备以下特性：

- **自动重启**: 服务异常退出时自动重启
- **启动限制**: 60秒内最多重启3次，防止频繁重启
- **资源限制**: 合理的文件句柄和进程数限制
- **安全隔离**: 私有临时目录、只读系统访问
- **日志管理**: 集成systemd日志系统
- **依赖管理**: 等待网络就绪后启动

### 服务配置文件

systemd服务配置位置：`/etc/systemd/system/tegrastats-api.service`

主要配置项：
```ini
[Service]
Type=simple                    # 前台运行模式
Restart=always                 # 总是重启
RestartSec=10                  # 重启间隔10秒
TimeoutStopSec=30             # 停止超时30秒

# 安全设置
NoNewPrivileges=true          # 不允许提权
PrivateTmp=true               # 私有临时目录
ProtectSystem=strict          # 严格系统保护

# 资源限制
LimitNOFILE=65536             # 文件句柄限制
LimitNPROC=4096               # 进程数限制
```

## �🔧 故障排除

### 常见问题

#### 1. 服务无法启动
```bash
# 检查端口占用
netstat -tlnp | grep 5000

# 检查tegrastats权限
sudo tegrastats --help
```

#### 2. ESP32S3连接失败
- 检查WiFi连接
- 确认服务器IP地址 (10.10.99.98)
- 验证防火墙设置

#### 3. 数据解析错误
- 确保JSON缓冲区足够大 (8KB+)
- 检查ArduinoJson库版本
- 验证数据格式

#### 4. WebSocket连接不稳定
- 增加重连间隔时间
- 检查网络质量
- 考虑使用REST API轮询方式

### 日志查看
```bash
# 查看服务日志
tail -f app.log

# 查看错误日志
grep ERROR app.log
```

## 📈 性能指标

- **API响应时间**: < 100ms
- **数据更新延迟**: < 50ms
- **内存占用**: ~50MB
- **CPU占用**: < 5%
- **网络带宽**: ~1KB/s (每客户端)

## 📄 许可证

本项目采用 MIT 许可证。