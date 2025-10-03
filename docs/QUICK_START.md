# Tegrastats API 快速部署指南

## 🚀 一键部署

### 前提条件
- NVIDIA Jetson设备 (Orin/Xavier/Nano等)
- Ubuntu 18.04/20.04/22.04
- 具有sudo权限的用户账户
- 网络连接

### 部署步骤

1. **下载项目**
```bash
git clone <repository-url>
cd rm01OrinStatus
```

2. **运行自动安装**
```bash
./install.sh
```

3. **验证安装**
```bash
# 检查服务状态
./service_control.sh status

# 测试API
./service_control.sh test
```

就这么简单！服务现在运行在 `http://10.10.99.98:5000`

## 📱 ESP32S3快速集成

### 1. Arduino IDE配置
- 安装ESP32开发板支持
- 安装 `ArduinoJson` 库
- 安装 `WebSockets` 库 (WebSocket版本)

### 2. 使用示例代码
- REST版本: `esp32s3_example.cpp`
- WebSocket版本: `esp32s3_websocket_example.cpp`

### 3. 修改配置
```cpp
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";
const char* jetson_ip = "10.10.99.98";  // 如果IP不同需要修改
```

## 🔧 常用命令

### 服务管理
```bash
./service_control.sh start     # 启动服务
./service_control.sh stop      # 停止服务
./service_control.sh restart   # 重启服务
./service_control.sh logs      # 查看日志
./service_control.sh test      # 测试API
```

### API测试
```bash
# 健康检查
curl http://10.10.99.98:5000/api/health

# 获取系统状态
curl http://10.10.99.98:5000/api/status

# 获取CPU信息
curl http://10.10.99.98:5000/api/cpu
```

## 🆘 故障排除

### 常见问题

**Q: 安装失败，提示conda未找到**
```bash
# 手动安装Miniconda后重新运行
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-aarch64.sh
bash Miniconda3-latest-Linux-aarch64.sh
source ~/.bashrc
./install.sh
```

**Q: 服务无法启动**
```bash
# 查看详细错误信息
sudo journalctl -u tegrastats-api -f

# 检查端口占用
sudo netstat -tlnp | grep 5000
```

**Q: API返回空数据**
```bash
# 检查tegrastats权限
sudo tegrastats --help

# 手动测试tegrastats
tegrastats --interval 1000 --logfile /tmp/test.log &
```

**Q: ESP32S3连接失败**
- 检查WiFi连接
- 确认Jetson IP地址
- 检查防火墙设置

### 获取帮助
```bash
./service_control.sh help     # 查看管理脚本帮助
./install.sh --help          # 查看安装选项
```

## 🗑️ 完全卸载

如需移除所有组件：
```bash
./uninstall.sh
```

这将删除：
- systemd服务
- conda虚拟环境
- 配置文件
- 临时文件

项目源代码将被保留。

---

更多详细信息请参考 [完整文档](README.md)