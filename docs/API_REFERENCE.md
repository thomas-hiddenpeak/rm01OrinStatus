# Tegrastats API 参考文档

本文档详细描述了 Tegrastats API 的所有接口和功能。

## 📋 目录

- [概述](#概述)
- [REST API](#rest-api)
- [WebSocket API](#websocket-api)
- [Python API](#python-api)
- [CLI命令](#cli命令)
- [数据格式](#数据格式)
- [错误处理](#错误处理)

## 概述

Tegrastats API 提供了三种访问方式：

1. **REST API**: HTTP端点，适合单次查询
2. **WebSocket API**: 实时数据推送，适合持续监控
3. **Python API**: 程序库接口，适合集成开发

## REST API

### 基础信息

- **基础URL**: `http://{host}:{port}/api`
- **默认地址**: `http://10.10.99.98:58090/api`
- **内容类型**: `application/json`
- **字符编码**: `UTF-8`

### 端点列表

#### 1. 健康检查

获取服务器状态和连接信息。

```http
GET /api/health
```

**响应示例**:
```json
{
  "status": "healthy",
  "service": "tegrastats-api",
  "timestamp": "2025-10-03T06:33:33.964139Z",
  "connected_clients": 2
}
```

#### 2. 完整系统状态

获取所有系统监控数据。

```http
GET /api/status
```

**响应示例**:
```json
{
  "cpu": {
    "cores": [
      {"id": 0, "usage": 15, "freq": 1200},
      {"id": 1, "usage": 8, "freq": 1200}
    ]
  },
  "gpu": {
    "gr3d_freq": 850
  },
  "memory": {
    "ram": {
      "used": 2514,
      "total": 62841,
      "unit": "MB"
    },
    "swap": {
      "used": 0,
      "total": 31421,
      "unit": "MB",
      "cached": 0
    }
  },
  "temperature": {
    "cpu": 47.125,
    "soc0": 45.0,
    "soc1": 46.062,
    "soc2": 45.562,
    "tj": 47.125
  },
  "power": {
    "ram": {
      "current": 2514,
      "average": 62841,
      "unit": "mW"
    },
    "swap": {
      "current": 0,
      "average": 31421,
      "unit": "mW"
    }
  },
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

#### 3. CPU信息

获取CPU使用率和频率信息。

```http
GET /api/cpu
```

**响应示例**:
```json
{
  "cpu": {
    "cores": [
      {"id": 0, "usage": 15, "freq": 1200},
      {"id": 1, "usage": 8, "freq": 1200},
      {"id": 2, "usage": 12, "freq": 1200}
    ]
  },
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

#### 4. 内存信息

获取RAM和SWAP使用情况。

```http
GET /api/memory
```

**响应示例**:
```json
{
  "memory": {
    "ram": {
      "used": 2514,
      "total": 62841,
      "unit": "MB"
    },
    "swap": {
      "used": 0,
      "total": 31421,
      "unit": "MB",
      "cached": 0
    }
  },
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

#### 5. 温度信息

获取各传感器温度数据。

```http
GET /api/temperature
```

**响应示例**:
```json
{
  "temperature": {
    "cpu": 47.125,
    "soc0": 45.0,
    "soc1": 46.062,
    "soc2": 45.562,
    "tj": 47.125
  },
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

#### 6. 功耗信息

获取各电源域功耗数据。

```http
GET /api/power
```

**响应示例**:
```json
{
  "power": {
    "ram": {
      "current": 2514,
      "average": 62841,
      "unit": "mW"
    },
    "swap": {
      "current": 0,
      "average": 31421,
      "unit": "mW"
    }
  },
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

### HTTP状态码

- `200 OK`: 请求成功
- `503 Service Unavailable`: 数据不可用（tegrastats未运行）
- `500 Internal Server Error`: 服务器内部错误

## WebSocket API

### 连接信息

- **URL**: `ws://{host}:{port}/socket.io/`
- **协议**: Socket.IO
- **传输**: WebSocket
- **默认地址**: `ws://10.10.99.98:58090/socket.io/`

### 连接限制

- **最大连接数**: 10（可配置）
- **连接超时**: 30秒
- **数据推送频率**: 1Hz（每秒1次）

### 事件

#### 连接事件

```javascript
// 连接成功
socket.on('connect', function() {
    console.log('Connected to Tegrastats API');
});

// 连接断开
socket.on('disconnect', function() {
    console.log('Disconnected from Tegrastats API');
});
```

#### 数据更新事件

```javascript
// 接收实时数据更新
socket.on('tegrastats_update', function(data) {
    console.log('Received update:', data);
    // data 格式与 /api/status 相同
});
```

### 客户端示例

#### JavaScript (浏览器)

```html
<script src="/socket.io/socket.io.js"></script>
<script>
const socket = io();

socket.on('connect', function() {
    console.log('Connected');
});

socket.on('tegrastats_update', function(data) {
    console.log('CPU Usage:', data.cpu.cores[0].usage + '%');
    console.log('Memory:', data.memory.ram.used + '/' + data.memory.ram.total + ' MB');
    console.log('Temperature:', data.temperature.cpu + '°C');
});
</script>
```

#### Python

```python
import socketio

sio = socketio.Client()

@sio.event
def connect():
    print('Connected to server')

@sio.event
def tegrastats_update(data):
    print(f"CPU: {data['cpu']['cores'][0]['usage']}%")
    print(f"Memory: {data['memory']['ram']['used']}MB")
    print(f"Temperature: {data['temperature']['cpu']}°C")

sio.connect('http://10.10.99.98:58090')
sio.wait()
```

## Python API

### 核心类

#### TegrastatsServer

主服务器类，提供HTTP和WebSocket服务。

```python
from tegrastats_api import TegrastatsServer, Config

# 基本使用
server = TegrastatsServer()
server.run()

# 自定义配置
config = Config(host='0.0.0.0', port=8080)
server = TegrastatsServer(config)

# 上下文管理器
with TegrastatsServer(config) as server:
    # 自动启动和停止
    pass
```

**方法**:
- `run(**kwargs)`: 运行服务器
- `start()`: 启动服务器组件
- `stop()`: 停止服务器组件

#### TegrastatsParser

tegrastats数据解析器。

```python
from tegrastats_api import TegrastatsParser

parser = TegrastatsParser(interval=2.0)

with parser:
    status = parser.get_current_status()
    print(status)
```

**方法**:
- `start()`: 启动解析器
- `stop()`: 停止解析器
- `get_current_status()`: 获取当前状态数据

#### Config

配置管理类。

```python
from tegrastats_api import Config

config = Config(
    host='127.0.0.1',
    port=58090,
    debug=False,
    log_level='INFO',
    max_connections=10,
    update_interval=1.0,
    tegrastats_interval=1.0,
    cors_origins='*'
)
```

**属性**:
- `host`: 绑定IP地址
- `port`: 绑定端口
- `debug`: 调试模式
- `log_level`: 日志级别
- `max_connections`: 最大WebSocket连接数
- `update_interval`: 数据更新间隔
- `tegrastats_interval`: tegrastats采样间隔
- `cors_origins`: CORS允许的源

## CLI命令

### 主命令

```bash
tegrastats-api [OPTIONS] COMMAND [ARGS]...
```

**全局选项**:
- `--version`: 显示版本信息
- `--help`: 显示帮助信息

### 子命令

#### run - 启动服务器

```bash
tegrastats-api run [OPTIONS]
```

**选项**:
- `-h, --host TEXT`: 绑定IP地址
- `-p, --port INTEGER`: 绑定端口
- `--debug`: 启用调试模式
- `--log-level [DEBUG|INFO|WARNING|ERROR]`: 日志级别
- `--max-connections INTEGER`: 最大WebSocket连接数
- `--update-interval FLOAT`: 数据更新间隔(秒)
- `--tegrastats-interval FLOAT`: Tegrastats采样间隔(秒)

**示例**:
```bash
tegrastats-api run
tegrastats-api run --host 0.0.0.0 --port 8080 --debug
```

#### config - 显示配置

```bash
tegrastats-api config
```

显示当前配置信息。

#### test - 测试连接

```bash
tegrastats-api test [OPTIONS]
```

**选项**:
- `-h, --host TEXT`: 服务器地址 (默认: localhost)
- `-p, --port INTEGER`: 服务器端口 (默认: 58090)
- `-e, --endpoint TEXT`: API端点 (默认: /api/status)

**示例**:
```bash
tegrastats-api test
tegrastats-api test --host 192.168.1.100 --port 8080
tegrastats-api test --endpoint cpu
```

#### monitor - 实时监控

```bash
tegrastats-api monitor [OPTIONS]
```

**选项**:
- `-h, --host TEXT`: 服务器地址 (默认: localhost)
- `-p, --port INTEGER`: 服务器端口 (默认: 58090)
- `-d, --duration INTEGER`: 监听时长(秒) (默认: 10)

**示例**:
```bash
tegrastats-api monitor
tegrastats-api monitor --duration 30
```

## 数据格式

### 时间戳格式

所有API响应都包含ISO 8601格式的UTC时间戳：

```json
{
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

### CPU数据格式

```json
{
  "cpu": {
    "cores": [
      {
        "id": 0,        // 核心ID
        "usage": 15,    // 使用率百分比
        "freq": 1200    // 频率(MHz)
      }
    ]
  }
}
```

### 内存数据格式

```json
{
  "memory": {
    "ram": {
      "used": 2514,     // 已使用内存
      "total": 62841,   // 总内存
      "unit": "MB"      // 单位
    },
    "swap": {
      "used": 0,        // 已使用交换空间
      "total": 31421,   // 总交换空间
      "unit": "MB",     // 单位
      "cached": 0       // 缓存大小
    }
  }
}
```

### 温度数据格式

```json
{
  "temperature": {
    "cpu": 47.125,    // CPU温度(°C)
    "soc0": 45.0,     // SoC温度传感器0
    "soc1": 46.062,   // SoC温度传感器1
    "soc2": 45.562,   // SoC温度传感器2
    "tj": 47.125      // 结温
  }
}
```

### 功耗数据格式

```json
{
  "power": {
    "ram": {
      "current": 2514,    // 当前功耗
      "average": 62841,   // 平均功耗
      "unit": "mW"        // 单位(毫瓦)
    }
  }
}
```

## 错误处理

### HTTP错误响应

```json
{
  "error": "Error message description",
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

### 常见错误

#### 503 Service Unavailable

```json
{
  "error": "No data available",
  "timestamp": "2025-10-03T06:33:49.223455Z"
}
```

**原因**: tegrastats进程未运行或数据不可用

**解决**: 检查tegrastats命令是否可用，确保在Jetson设备上运行

#### WebSocket连接被拒绝

**原因**: 达到最大连接数限制

**解决**: 等待其他连接断开或增加最大连接数配置

### 调试建议

1. **启用调试模式**: `--debug --log-level DEBUG`
2. **检查日志输出**: 查看详细的错误信息
3. **验证tegrastats**: 手动运行 `tegrastats` 命令
4. **检查网络**: 确保端口未被占用
5. **权限检查**: 确保有足够的权限访问系统资源

---

更多信息请参考 [安装指南](INSTALL_GUIDE.md) 和项目README。