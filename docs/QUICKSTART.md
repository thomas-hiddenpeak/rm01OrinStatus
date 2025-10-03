# 快速开始指南

欢迎使用 Tegrastats API！本指南将帮助您在5分钟内完成安装和配置。

## 📋 前置条件

- NVIDIA Jetson设备 (任何型号)
- Ubuntu 18.04+ 或 JetPack 4.x+
- Python 3.9+ 和 conda
- 系统管理员权限

## 🚀 一键安装

### 步骤1：克隆项目
```bash
git clone https://github.com/thomas-hiddenpeak/rm01OrinStatus.git
cd rm01OrinStatus
```

### 步骤2：运行安装脚本
```bash
./scripts/install.sh
```

这将自动：
- ✅ 创建conda环境 `tegrastats-api`
- ✅ 安装所有Python依赖
- ✅ 配置systemd服务
- ✅ 启动API服务
- ✅ 启用开机自启动

### 步骤3：验证安装
```bash
# 检查服务状态
./scripts/manage_tegrastats.sh status

# 测试API连接
curl http://localhost:58090/api/health
```

预期输出：
```json
{
  "connected_clients": 0,
  "service": "tegrastats-api", 
  "status": "healthy",
  "timestamp": "2025-10-03T10:48:29.461151Z"
}
```

## 🧪 快速测试

### 测试REST API
```bash
# 获取系统状态
curl http://localhost:58090/api/status | python -m json.tool

# 获取CPU信息
curl http://localhost:58090/api/cpu | python -m json.tool
```

### 测试WebSocket
```bash
# 运行WebSocket测试
python tests/test_simple_ws.py
```

## 🔧 基本使用

### 服务管理
```bash
# 启动服务
./scripts/manage_tegrastats.sh start

# 停止服务  
./scripts/manage_tegrastats.sh stop

# 重启服务
./scripts/manage_tegrastats.sh restart

# 查看日志
./scripts/manage_tegrastats.sh logs
```

### API端点
| 端点 | 描述 |
|------|------|
| `/api/health` | 服务健康检查 |
| `/api/status` | 完整系统状态 |
| `/api/cpu` | CPU使用率和频率 |
| `/api/memory` | 内存使用情况 |
| `/api/temperature` | 温度传感器数据 |
| `/api/power` | 功耗监控数据 |

### WebSocket连接
```javascript
// JavaScript示例
const socket = io('http://localhost:58090');
socket.on('tegrastats_update', (data) => {
    console.log('实时数据:', data);
});
```

## 🎯 下一步

- 查看 [API参考文档](API_REFERENCE.md)
- 探索 [示例代码](../examples/)
- 阅读 [部署指南](DEPLOYMENT_GUIDE.md)
- 了解 [Wheel包构建](WHEEL_BUILD_GUIDE.md)
- 参与 [项目贡献](CONTRIBUTING.md)

## 🆘 遇到问题？

### 常见问题
1. **服务启动失败**: 检查 `sudo journalctl -u tegrastats-api`
2. **端口占用**: 使用 `./scripts/install.sh --port 8080` 指定其他端口
3. **权限问题**: 确保有sudo权限并且tegrastats命令可用

### 获取帮助
- 查看 [GitHub Issues](https://github.com/thomas-hiddenpeak/rm01OrinStatus/issues)
- 参考 [故障排除指南](docs/TROUBLESHOOTING.md)
- 联系维护者

---

🎉 **恭喜！您的Tegrastats API已成功运行！**