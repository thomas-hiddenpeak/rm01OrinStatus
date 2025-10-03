# Tegrastats API 安装指南

## 安装方式说明

Tegrastats API 支持多种安装方式，每种方式适合不同的使用场景：

### 1. Wheel包安装（推荐）

使用预构建的wheel包进行安装：

```bash
# 从本地wheel包安装
pip install dist/tegrastats_api-1.0.0-py3-none-any.whl

# 或从GitHub Releases下载安装
pip install https://github.com/thomas-hiddenpeak/rm01OrinStatus/releases/download/v1.0.0/tegrastats_api-1.0.0-py3-none-any.whl

# 验证安装
tegrastats-api --version
tegrastats-api test
```

**优点：**
- 安装速度最快
- 不需要构建环境
- 依赖自动管理
- 支持离线安装

### 2. 专用Conda环境安装

创建专用环境，与其他项目隔离：

```bash
# 创建专用环境
conda create -n tegrastats-api python=3.9 -y
conda activate tegrastats-api

# 安装包
pip install -e .

# 创建系统服务
sudo ./create_service.sh --host 0.0.0.0 --port 58090
```

**优点：**
- 环境隔离，不影响其他项目
- 依赖版本完全独立
- 卸载时可以完全删除环境

**缺点：**
- 占用额外磁盘空间
- 需要切换环境

### 2. 当前Conda环境安装

在已有的conda环境中安装：

```bash
# 在当前环境中安装
pip install -e .

# 创建系统服务（自动检测当前环境）
sudo ./create_service.sh --host 0.0.0.0 --port 58090
```

**优点：**
- 不需要创建新环境
- 节省磁盘空间
- 可以与其他项目共享依赖

**缺点：**
- 可能存在依赖冲突
- 卸载时需要小心，避免影响其他项目

### 3. Base环境安装

在conda的base环境中安装：

```bash
# 激活base环境
conda activate base

# 安装包
pip install -e .

# 创建系统服务
sudo ./create_service.sh --host 0.0.0.0 --port 58090
```

**注意：** 不推荐在base环境中安装项目依赖，因为这可能影响conda的正常工作。

## 安装检测机制

### create_service.sh 自动检测逻辑：

1. **当前PATH检测：** 首先检查 `tegrastats-api` 命令是否在当前PATH中可用
2. **Conda环境检测：** 检查当前是否在conda环境中，并检测环境路径
3. **用户环境搜索：** 搜索用户的conda环境目录，查找已安装的包
4. **虚拟环境检测：** 检查常见的virtualenv位置

### 服务文件生成：

服务文件会记录检测到的Python环境路径：

```ini
# 示例：专用环境
Environment=PATH=/home/user/miniconda3/envs/tegrastats-api/bin:/usr/local/bin:/usr/bin:/bin
ExecStart=/home/user/miniconda3/envs/tegrastats-api/bin/tegrastats-api run --host 0.0.0.0 --port 58090

# 示例：base环境
Environment=PATH=/home/user/miniconda3/bin:/usr/local/bin:/usr/bin:/bin
ExecStart=/home/user/miniconda3/bin/tegrastats-api run --host 0.0.0.0 --port 58090
```

## 卸载处理策略

### uninstall.sh 智能卸载：

1. **环境类型检测：** 从systemd服务文件中检测实际使用的环境类型
2. **安全确认：** 
   - 专用环境：询问是否删除整个环境
   - Base环境：只卸载包，保留环境
   - 现有环境：只卸载包，保留环境

### 卸载流程：

```bash
# 运行卸载脚本
./uninstall.sh

# 脚本会显示：
# - 当前安装状态
# - 使用的环境类型
# - 将要执行的操作
# - 环境处理策略
```

## 最佳实践建议

### 开发环境：
- 使用专用conda环境
- 方便完全重装和测试

### 生产环境：
- 使用专用conda环境
- 配置系统服务自动启动
- 定期备份配置

### 临时测试：
- 可以使用当前环境
- 测试完成后及时清理

## 常见问题

### Q: 如何在不同环境之间切换？

```bash
# 停止当前服务
sudo systemctl stop tegrastats-api

# 卸载当前安装
./uninstall.sh

# 切换到新环境并重新安装
conda activate new-env
pip install -e .
sudo ./create_service.sh
```

### Q: 如何避免删除重要的conda环境？

卸载脚本会：
1. 检测环境类型
2. 如果是base环境，自动跳过删除
3. 如果是专用环境，询问确认
4. 显示环境中的所有包，帮助用户判断

### Q: 服务创建失败怎么办？

```bash
# 检查Python包是否正确安装
pip show tegrastats-api

# 检查CLI命令是否可用
which tegrastats-api

# 手动指定Python环境
sudo ./create_service.sh --python-env /path/to/your/env --host 0.0.0.0 --port 58090
```

## 环境变量配置

所有安装方式都支持通过环境变量配置：

```bash
# 在 ~/.bashrc 或 ~/.zshrc 中设置
export TEGRASTATS_API_HOST=0.0.0.0
export TEGRASTATS_API_PORT=58090
export TEGRASTATS_API_DEBUG=false
export TEGRASTATS_API_ALLOW_UNSAFE_WERKZEUG=true
```

这样就能确保无论使用哪种安装方式，都能正确配置和管理服务。