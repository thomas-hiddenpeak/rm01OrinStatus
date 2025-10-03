# ESP32S3 Jetson Monitor 库依赖配置

## Arduino IDE 库安装指南

### 必需库
1. **WiFi** (ESP32 Core内置)
   - 版本: 随ESP32核心包
   - 用途: WiFi连接管理

2. **HTTPClient** (ESP32 Core内置) 
   - 版本: 随ESP32核心包
   - 用途: REST API请求

3. **ArduinoJson** by Benoit Blanchon
   - 版本: 6.21.3 或更高
   - 安装: 库管理器搜索 "ArduinoJson"
   - 用途: JSON数据解析

4. **WebSockets** by Markus Sattler (仅WebSocket版本需要)
   - 版本: 2.3.6 或更高  
   - 安装: 库管理器搜索 "WebSockets"
   - 用途: WebSocket实时通信

### 板卡配置
- 板卡: ESP32S3 Dev Module
- CPU频率: 240MHz
- Flash模式: QIO
- Flash大小: 4MB (32Mb)
- 分区方案: Default 4MB with spiffs
- PSRAM: Enabled

### 编译配置
```
arduino-cli compile --fqbn esp32:esp32:esp32s3:
  CPUFreq=240,
  FlashMode=qio,
  FlashSize=4M,
  PartitionScheme=default,
  PSRAM=enabled
```

### 库版本兼容性
| 库名称 | 最低版本 | 推荐版本 | 备注 |
|--------|----------|----------|------|
| ArduinoJson | 6.19.0 | 6.21.3 | 支持大型JSON文档 |
| WebSockets | 2.3.0 | 2.3.6 | Socket.IO支持 |
| ESP32 Core | 2.0.0 | 2.0.11 | 稳定WiFi支持 |

### 内存使用优化
- JSON缓冲区: 8KB (可根据需要调整)
- WebSocket缓冲区: 默认
- 串口缓冲区: 1024字节
- WiFi缓冲区: 默认

### 故障排除
1. **编译错误**: 确保安装了正确版本的库
2. **连接失败**: 检查WiFi凭据和Jetson设备IP
3. **内存不足**: 启用PSRAM或减少缓冲区大小
4. **数据解析失败**: 检查JSON文档大小限制

### 性能监控
- 内存使用: 通过串口监视器观察堆内存
- 连接质量: 通过RSSI值和延迟指标评估
- 数据吞吐: 监控更新频率和丢包率