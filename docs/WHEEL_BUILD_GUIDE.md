# Tegrastats API Wheel 包构建与分发指南

## 🎯 概述

本项目已成功配置为可构建标准 Python wheel 包，支持通过 pip 安装和分发。

## 📦 构建 Wheel 包

### 自动构建（推荐）
```bash
# 使用构建脚本
./scripts/build_wheel.sh
```

### 手动构建
```bash
# 安装构建工具
pip install build

# 构建包
python -m build

# 查看生成的文件
ls -la dist/
```

## 🗂️ 生成的文件

构建完成后会在 `dist/` 目录生成：

```
dist/
├── tegrastats_api-1.0.0-py3-none-any.whl  # Wheel包（推荐安装方式）
└── tegrastats_api-1.0.0.tar.gz            # 源码包
```

## 💾 本地安装测试

### 从 Wheel 包安装
```bash
# 卸载现有版本（如果有）
pip uninstall tegrastats-api -y

# 安装 wheel 包
pip install dist/tegrastats_api-1.0.0-py3-none-any.whl

# 测试命令行工具
tegrastats-api --help
tegrastats-api test
```

### 从源码包安装
```bash
pip install dist/tegrastats_api-1.0.0.tar.gz
```

## 🚀 发布到 PyPI

### 1. 安装发布工具
```bash
pip install twine
```

### 2. 检查包
```bash
twine check dist/*
```

### 3. 上传到 Test PyPI（测试）
```bash
twine upload --repository-url https://test.pypi.org/legacy/ dist/*
```

### 4. 上传到 PyPI（正式发布）
```bash
twine upload dist/*
```

### 5. 从 PyPI 安装
```bash
# 从 Test PyPI 安装
pip install --index-url https://test.pypi.org/simple/ tegrastats-api

# 从 PyPI 安装
pip install tegrastats-api
```

## 📋 包信息

- **包名**: `tegrastats-api`
- **版本**: `1.0.0`
- **平台**: `py3-none-any` (支持所有Python 3.8+平台)
- **许可**: MIT
- **作者**: Thomas Hiddenpeak
- **项目地址**: https://github.com/thomas-hiddenpeak/rm01OrinStatus

## 🎯 包含的组件

### 命令行工具
- `tegrastats-api` - 主命令行工具
- `tegrastats-server` - 服务器别名

### Python 模块
```python
import tegrastats_api
from tegrastats_api import TegraStatsParser, TegraStatsServer
```

### 服务功能
- REST API 服务器
- WebSocket 实时数据推送
- 系统监控数据解析
- 配置管理

## 🔧 依赖管理

### 核心依赖
- Flask>=2.3.0
- Flask-SocketIO>=5.3.0  
- Flask-CORS>=4.0.0
- python-socketio>=5.9.0
- eventlet>=0.33.0
- psutil>=5.9.0
- requests>=2.31.0
- websocket-client>=1.6.0
- click>=8.0.0

### 开发依赖（可选）
```bash
pip install tegrastats-api[dev]
```

### 测试依赖（可选）
```bash
pip install tegrastats-api[test]
```

## 📝 版本管理

### 更新版本号
编辑 `pyproject.toml`:
```toml
[project]
version = "1.1.0"  # 更新版本号
```

### 构建新版本
```bash
./scripts/build_wheel.sh
```

## 🧪 质量检查

### 包完整性验证
```bash
# 检查包结构
python -m zipfile -l dist/tegrastats_api-1.0.0-py3-none-any.whl

# 安装测试
pip install dist/tegrastats_api-1.0.0-py3-none-any.whl --force-reinstall

# 功能测试
tegrastats-api --version
tegrastats-api test
```

### 元数据验证
```bash
# 检查包元数据
twine check dist/*

# 查看包信息
pip show tegrastats-api
```

## 🔄 自动化构建

项目已配置 GitHub Actions CI/CD，可以：
- 自动运行测试
- 自动构建 wheel 包
- 自动发布到 PyPI (需要配置密钥)

## 📚 相关文档

- [Python 打包用户指南](https://packaging.python.org/)
- [PyPI 发布指南](https://packaging.python.org/tutorials/packaging-projects/)
- [Wheel 格式规范](https://peps.python.org/pep-0427/)

---

## 🎉 成功案例

我们的项目现在可以：

1. ✅ **一键构建**: `./scripts/build_wheel.sh`
2. ✅ **标准安装**: `pip install tegrastats-api`
3. ✅ **命令行工具**: `tegrastats-api --help`
4. ✅ **Python 导入**: `import tegrastats_api`
5. ✅ **PyPI 发布**: 完整的发布流程
6. ✅ **跨平台兼容**: 支持所有 Python 3.8+ 平台

**项目已完全准备好进行商业分发和社区共享！** 🚀