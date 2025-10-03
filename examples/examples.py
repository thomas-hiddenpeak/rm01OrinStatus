#!/usr/bin/env python3
"""
Tegrastats API 使用示例

演示如何使用 Tegrastats API 作为 Python 库
"""

import time
import json
import logging
from tegrastats_api import TegrastatsServer, Config, TegrastatsParser

# 配置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def example_1_basic_usage():
    """示例1: 基本用法"""
    print("=== 示例1: 基本用法 ===")
    
    # 使用默认配置创建服务器
    server = TegrastatsServer()
    
    print(f"服务器配置: {server.config.host}:{server.config.port}")
    print("提示: 运行后访问 http://localhost:58090/api/health")
    print("按 Ctrl+C 停止服务器")
    
    try:
        server.run()
    except KeyboardInterrupt:
        print("用户中断，服务器已停止")

def example_2_custom_config():
    """示例2: 自定义配置"""
    print("=== 示例2: 自定义配置 ===")
    
    # 创建自定义配置
    config = Config(
        host='0.0.0.0',
        port=8080,
        debug=True,
        max_connections=5,
        update_interval=0.5  # 每0.5秒更新一次
    )
    
    server = TegrastatsServer(config)
    
    print(f"自定义服务器配置: {config.host}:{config.port}")
    print(f"调试模式: {config.debug}")
    print(f"最大连接数: {config.max_connections}")
    print(f"更新间隔: {config.update_interval}秒")
    
    try:
        server.run()
    except KeyboardInterrupt:
        print("用户中断，服务器已停止")

def example_3_context_manager():
    """示例3: 上下文管理器用法"""
    print("=== 示例3: 上下文管理器用法 ===")
    
    config = Config(host='127.0.0.1', port=9090, debug=False)
    
    print("使用上下文管理器，服务器会自动启动和停止")
    print(f"服务器将在 {config.host}:{config.port} 启动")
    
    try:
        with TegrastatsServer(config) as server:
            print("服务器已启动，等待5秒...")
            time.sleep(5)
            print("上下文结束，服务器将自动停止")
    except Exception as e:
        print(f"错误: {e}")

def example_4_parser_only():
    """示例4: 仅使用解析器"""
    print("=== 示例4: 仅使用tegrastats解析器 ===")
    
    # 只使用解析器组件，不启动HTTP服务器
    parser = TegrastatsParser(interval=2.0)  # 2秒间隔
    
    print("启动tegrastats解析器...")
    
    try:
        with parser:
            for i in range(5):
                print(f"\n--- 第{i+1}次读取 ---")
                
                # 等待数据
                time.sleep(2.5)
                
                # 获取当前状态
                status = parser.get_current_status()
                
                if status:
                    # 显示关键信息
                    if 'cpu' in status and 'cores' in status['cpu']:
                        cores = status['cpu']['cores']
                        avg_usage = sum(core['usage'] for core in cores) / len(cores)
                        print(f"CPU平均使用率: {avg_usage:.1f}%")
                    
                    if 'memory' in status and 'ram' in status['memory']:
                        ram = status['memory']['ram']
                        usage_pct = (ram['used'] / ram['total']) * 100
                        print(f"内存使用: {ram['used']}/{ram['total']} MB ({usage_pct:.1f}%)")
                    
                    if 'temperature' in status:
                        temps = status['temperature']
                        if 'cpu' in temps:
                            print(f"CPU温度: {temps['cpu']}°C")
                    
                else:
                    print("暂无数据")
                
    except KeyboardInterrupt:
        print("\n用户中断")

def main():
    """主函数"""
    print("Tegrastats API Python库使用示例")
    print("=================================")
    
    examples = {
        '1': ('基本用法（默认配置）', example_1_basic_usage),
        '2': ('自定义配置', example_2_custom_config),
        '3': ('上下文管理器', example_3_context_manager),
        '4': ('仅使用解析器', example_4_parser_only),
    }
    
    print("\n可用示例:")
    for key, (desc, _) in examples.items():
        print(f"  {key}. {desc}")
    
    try:
        choice = input("\n请选择示例编号 (1-4): ").strip()
        
        if choice in examples:
            desc, func = examples[choice]
            print(f"\n执行示例: {desc}")
            print("-" * 40)
            func()
        else:
            print("无效选择")
            
    except KeyboardInterrupt:
        print("\n\n程序被中断")
    except Exception as e:
        print(f"\n错误: {e}")

if __name__ == '__main__':
    main()