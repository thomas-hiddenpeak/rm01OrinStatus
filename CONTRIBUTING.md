# Contributing to Tegrastats API

我们欢迎社区贡献！以下是参与项目的指南。

## 开发环境设置

1. **克隆仓库**
   ```bash
   git clone https://github.com/your-username/tegrastats-api.git
   cd tegrastats-api
   ```

2. **设置开发环境**
   ```bash
   # 创建conda环境
   conda create -n tegrastats-api-dev python=3.9 -y
   conda activate tegrastats-api-dev
   
   # 安装开发依赖
   pip install -e .
   pip install pytest black flake8
   ```

3. **验证安装**
   ```bash
   ./scripts/full_verification.sh --auto
   ```

## 贡献流程

### 1. 问题报告
- 使用GitHub Issues报告bug
- 提供详细的重现步骤
- 包含系统信息和错误日志

### 2. 功能建议
- 在Issues中描述新功能需求
- 说明使用场景和预期效果
- 讨论实现方案

### 3. 代码贡献

#### 分支策略
- `main`: 稳定版本，用于发布
- `develop`: 开发分支，新功能集成
- `feature/*`: 功能分支
- `bugfix/*`: 修复分支

#### 提交流程
1. Fork项目
2. 创建功能分支: `git checkout -b feature/new-feature`
3. 进行开发和测试
4. 提交代码: `git commit -m "feat: add new feature"`
5. 推送分支: `git push origin feature/new-feature`
6. 创建Pull Request

#### 代码规范
- 遵循PEP 8 Python代码规范
- 使用有意义的变量和函数名
- 添加必要的注释和文档字符串
- 保持函数简洁，单一职责

#### 提交信息格式
使用约定式提交格式：
- `feat`: 新功能
- `fix`: 修复bug
- `docs`: 文档更新
- `style`: 代码格式化
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建过程或辅助工具的变动

示例：
```
feat: add WebSocket real-time data streaming
fix: resolve memory leak in parser thread
docs: update API reference for new endpoints
```

## 测试

### 运行测试
```bash
# 运行所有测试
python -m pytest tests/

# 运行特定测试
python tests/test_simple_ws.py

# 完整验证
./scripts/full_verification.sh
```

### 测试覆盖率
- 新功能需要包含测试用例
- 保持测试覆盖率在80%以上
- 测试关键功能和边界情况

## 文档

### 文档结构
- `README.md`: 项目概述和快速开始
- `docs/`: 详细文档
  - `API_REFERENCE.md`: API参考
  - `INSTALLATION_GUIDE.md`: 安装指南
  - `DEPLOYMENT_GUIDE.md`: 部署指南

### 文档更新
- 新功能需要更新相应文档
- 保持文档与代码同步
- 使用清晰的示例和说明

## 发布流程

### 版本号规则
使用语义化版本 (SemVer):
- `MAJOR.MINOR.PATCH`
- MAJOR: 不兼容的API变更
- MINOR: 向后兼容的功能添加
- PATCH: 向后兼容的问题修复

### 发布检查清单
- [ ] 所有测试通过
- [ ] 文档更新完成
- [ ] 版本号更新
- [ ] CHANGELOG.md更新
- [ ] 创建Git标签
- [ ] 发布到GitHub Releases

## 社区指南

### 行为准则
- 尊重他人，保持友善
- 鼓励新贡献者参与
- 专注于技术讨论
- 避免人身攻击和歧视

### 交流渠道
- GitHub Issues: 问题讨论和追踪
- GitHub Discussions: 一般讨论和问答
- Pull Requests: 代码审查和讨论

## 获取帮助

如果您在贡献过程中遇到问题，可以：

1. 查看现有的Issues和文档
2. 在GitHub Discussions中提问
3. 联系维护者

感谢您的贡献！🎉