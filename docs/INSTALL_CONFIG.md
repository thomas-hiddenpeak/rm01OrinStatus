# Tegrastats API 安装配置示例

## 默认安装
```bash
./install.sh
```
- 监听地址: 10.10.99.98:58090
- 适用于标准Jetson网络配置

## 监听所有接口
```bash
./install.sh --host 0.0.0.0
```
- 监听地址: 0.0.0.0:58090
- 允许从任何网络接口访问

## 自定义端口
```bash
./install.sh --port 8080
```
- 监听地址: 10.10.99.98:8080
- 使用标准HTTP端口

## 完全自定义
```bash
./install.sh --host 192.168.1.100 --port 9000
```
- 监听地址: 192.168.1.100:9000
- 适用于特定网络环境

## 生产环境建议

### 内网部署
```bash
./install.sh --host 192.168.1.100 --port 58090
```

### 开发测试
```bash
./install.sh --host 127.0.0.1 --port 58090
```
只允许本地访问

### 公网部署（需要额外安全措施）
```bash
./install.sh --host 0.0.0.0 --port 58090
```
⚠️ 注意：公网部署需要配置防火墙和认证

## 重新配置

如需更改配置：
1. 运行 `./uninstall.sh` 卸载现有服务
2. 使用新参数运行 `./install.sh --host NEW_HOST --port NEW_PORT`

## 配置验证

安装完成后，以下文件会自动更新：
- `config.py` - Python配置文件
- `service_control.sh` - 服务管理脚本
- `test_simple_ws.py` - WebSocket测试脚本