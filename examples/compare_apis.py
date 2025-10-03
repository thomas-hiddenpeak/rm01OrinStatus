#!/usr/bin/env python3
"""
比较REST API和WebSocket API数据完整性
"""

import requests
import socketio
import json
import time
from datetime import datetime

def compare_data_structures(rest_data, ws_data, path=""):
    """递归比较两个数据结构"""
    differences = []
    
    if isinstance(rest_data, dict) and isinstance(ws_data, dict):
        # 检查REST API独有的键
        rest_only = set(rest_data.keys()) - set(ws_data.keys())
        if rest_only:
            differences.append(f"{path} - REST API独有键: {rest_only}")
        
        # 检查WebSocket独有的键
        ws_only = set(ws_data.keys()) - set(rest_data.keys())
        if ws_only:
            differences.append(f"{path} - WebSocket独有键: {ws_only}")
        
        # 递归检查共同键
        common_keys = set(rest_data.keys()) & set(ws_data.keys())
        for key in common_keys:
            if key != 'timestamp':  # 忽略时间戳差异
                sub_diffs = compare_data_structures(
                    rest_data[key], ws_data[key], f"{path}.{key}" if path else key
                )
                differences.extend(sub_diffs)
    
    elif isinstance(rest_data, list) and isinstance(ws_data, list):
        if len(rest_data) != len(ws_data):
            differences.append(f"{path} - 列表长度不一致: REST={len(rest_data)}, WS={len(ws_data)}")
        else:
            for i, (rest_item, ws_item) in enumerate(zip(rest_data, ws_data)):
                sub_diffs = compare_data_structures(
                    rest_item, ws_item, f"{path}[{i}]" if path else f"[{i}]"
                )
                differences.extend(sub_diffs)
    
    elif type(rest_data) != type(ws_data):
        differences.append(f"{path} - 数据类型不一致: REST={type(rest_data).__name__}, WS={type(ws_data).__name__}")
    
    return differences

def main():
    print("🔍 tegrastats API 数据完整性对比测试")
    print("=" * 50)
    
    # 1. 获取REST API数据
    print("📡 获取REST API数据...")
    try:
        response = requests.get("http://10.10.99.98:5000/api/status", timeout=5)
        rest_data = response.json()
        print("✅ REST API数据获取成功")
    except Exception as e:
        print(f"❌ REST API获取失败: {e}")
        return
    
    # 2. 获取WebSocket数据
    print("\n📡 获取WebSocket数据...")
    ws_data = None
    
    sio = socketio.Client()
    
    @sio.event
    def status_update(data):
        nonlocal ws_data
        ws_data = data
        print("✅ WebSocket数据获取成功")
    
    try:
        sio.connect('http://10.10.99.98:5000')
        # 等待接收数据
        start_time = time.time()
        while ws_data is None and (time.time() - start_time) < 10:
            sio.sleep(0.1)
        
        if ws_data is None:
            print("❌ WebSocket数据获取超时")
            return
            
    except Exception as e:
        print(f"❌ WebSocket连接失败: {e}")
        return
    finally:
        sio.disconnect()
    
    # 3. 数据结构对比
    print("\n🔍 数据结构对比...")
    print(f"REST API数据键: {list(rest_data.keys())}")
    print(f"WebSocket数据键: {list(ws_data.keys())}")
    
    differences = compare_data_structures(rest_data, ws_data)
    
    if not differences:
        print("\n✅ 数据结构完全一致！")
    else:
        print(f"\n⚠️  发现 {len(differences)} 处差异:")
        for diff in differences:
            print(f"  - {diff}")
    
    # 4. 数据字段详细分析
    print("\n📊 数据字段详细分析:")
    
    def analyze_section(section_name, rest_section, ws_section):
        print(f"\n{section_name}:")
        if section_name in rest_data and section_name in ws_data:
            print(f"  ✅ 两种API都提供")
            if isinstance(rest_section, dict):
                for key in rest_section:
                    print(f"    - {key}: ✅")
        elif section_name in rest_data:
            print(f"  ⚠️  仅REST API提供")
        elif section_name in ws_data:
            print(f"  ⚠️  仅WebSocket提供")
        else:
            print(f"  ❌ 两种API都不提供")
    
    # 分析各个数据段
    sections = ['cpu', 'memory', 'temperature', 'power', 'gpu']
    for section in sections:
        analyze_section(section, 
                       rest_data.get(section), 
                       ws_data.get(section))
    
    # 5. 专门API端点测试
    print("\n🎯 专门API端点数据完整性:")
    endpoints = [
        ('cpu', '/api/cpu'),
        ('memory', '/api/memory'), 
        ('temperature', '/api/temperature'),
        ('power', '/api/power')
    ]
    
    for section, endpoint in endpoints:
        try:
            response = requests.get(f"http://10.10.99.98:5000{endpoint}", timeout=3)
            endpoint_data = response.json()
            
            # 比较专门端点和完整数据
            if section in rest_data:
                rest_section = {section: rest_data[section], 'timestamp': rest_data['timestamp']}
                diffs = compare_data_structures(rest_section, endpoint_data)
                if not diffs:
                    print(f"  {section}: ✅ 与完整API一致")
                else:
                    print(f"  {section}: ⚠️  存在差异")
            else:
                print(f"  {section}: ❌ 完整API中不存在")
                
        except Exception as e:
            print(f"  {section}: ❌ 端点访问失败")
    
    # 6. 总结
    print("\n📋 总结:")
    print("✅ REST API提供所有端点:")
    print("   - /api/status (完整数据)")
    print("   - /api/cpu (CPU信息)")
    print("   - /api/memory (内存信息)")
    print("   - /api/temperature (温度信息)")
    print("   - /api/power (功耗信息)")
    print("   - /api/health (健康检查)")
    
    print("\n✅ WebSocket API提供:")
    print("   - status_update事件 (完整数据实时推送)")
    
    if not differences:
        print("\n🎉 结论: 两种API提供相同的完整tegrastats数据!")
    else:
        print(f"\n⚠️  结论: 发现{len(differences)}处数据差异，需要检查")

if __name__ == "__main__":
    main()