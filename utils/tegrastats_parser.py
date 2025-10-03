import subprocess
import re
import json
from datetime import datetime
from typing import Dict, List, Optional
import logging

logger = logging.getLogger(__name__)


class TegrastatsParser:
    """
    tegrastats输出解析器
    将tegrastats的原始输出转换为结构化的JSON数据
    """
    
    def __init__(self, interval: int = 1000):
        """
        初始化解析器
        
        Args:
            interval: tegrastats采样间隔（毫秒）
        """
        self.interval = interval
        self.process = None
        
    def start_tegrastats(self) -> bool:
        """
        启动tegrastats进程
        
        Returns:
            bool: 启动是否成功
        """
        try:
            self.process = subprocess.Popen(
                ['tegrastats', '--interval', str(self.interval)],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                universal_newlines=True,
                bufsize=1
            )
            logger.info(f"tegrastats进程已启动，间隔: {self.interval}ms")
            return True
        except Exception as e:
            logger.error(f"启动tegrastats失败: {e}")
            return False
    
    def stop_tegrastats(self):
        """停止tegrastats进程"""
        if self.process:
            self.process.terminate()
            self.process.wait()
            self.process = None
            logger.info("tegrastats进程已停止")
    
    def get_single_reading(self) -> Optional[str]:
        """
        获取单次tegrastats读数
        
        Returns:
            str: tegrastats输出行，如果获取失败返回None
        """
        try:
            # 使用更短的超时时间获取单次读数
            result = subprocess.run(
                ['timeout', '1.5', 'tegrastats', '--interval', '500'],  # 使用500ms间隔，1.5s超时
                capture_output=True,
                text=True,
                timeout=2
            )
            
            if result.stdout:
                lines = result.stdout.strip().split('\n')
                # 返回最后一行有效数据
                for line in reversed(lines):
                    if line.strip() and 'RAM' in line:
                        return line.strip()
            
            return None
        except Exception as e:
            logger.error(f"获取tegrastats读数失败: {e}")
            return None
    
    def parse_line(self, line: str) -> Optional[Dict]:
        """
        解析单行tegrastats输出
        
        Args:
            line: tegrastats输出行
            
        Returns:
            Dict: 解析后的结构化数据
        """
        if not line or 'RAM' not in line:
            return None
            
        try:
            data = {
                'timestamp': datetime.now().isoformat() + 'Z',
                'memory': {},
                'cpu': {'cores': []},
                'temperature': {},
                'power': {},
                'gpu': {}
            }
            
            # 解析内存信息
            ram_match = re.search(r'RAM (\d+)/(\d+)MB', line)
            if ram_match:
                data['memory']['ram'] = {
                    'used': int(ram_match.group(1)),
                    'total': int(ram_match.group(2)),
                    'unit': 'MB'
                }
            
            # 解析SWAP信息
            swap_match = re.search(r'SWAP (\d+)/(\d+)MB \(cached (\d+)MB\)', line)
            if swap_match:
                data['memory']['swap'] = {
                    'used': int(swap_match.group(1)),
                    'total': int(swap_match.group(2)),
                    'cached': int(swap_match.group(3)),
                    'unit': 'MB'
                }
            
            # 解析CPU信息
            cpu_pattern = r'CPU \[(.*?)\]'
            cpu_match = re.search(cpu_pattern, line)
            if cpu_match:
                cpu_data = cpu_match.group(1)
                # 匹配每个CPU核心的使用率和频率
                core_pattern = r'(\d+)%@(\d+)'
                cores = re.findall(core_pattern, cpu_data)
                
                for i, (usage, freq) in enumerate(cores):
                    data['cpu']['cores'].append({
                        'id': i,
                        'usage': int(usage),
                        'freq': int(freq)
                    })
            
            # 解析GPU频率
            gpu_match = re.search(r'GR3D_FREQ (\d+)%', line)
            if gpu_match:
                data['gpu']['gr3d_freq'] = int(gpu_match.group(1))
            
            # 解析温度信息
            temp_patterns = {
                'cpu': r'cpu@([\d.]+)C',
                'soc0': r'soc0@([\d.]+)C',
                'soc1': r'soc1@([\d.]+)C',
                'soc2': r'soc2@([\d.]+)C',
                'tj': r'tj@([\d.]+)C'
            }
            
            for key, pattern in temp_patterns.items():
                match = re.search(pattern, line)
                if match:
                    data['temperature'][key] = float(match.group(1))
            
            # 解析功耗信息
            power_patterns = {
                'gpu_soc': r'VDD_GPU_SOC (\d+)mW/(\d+)mW',
                'cpu_cv': r'VDD_CPU_CV (\d+)mW/(\d+)mW',
                'sys_5v': r'VIN_SYS_5V0 (\d+)mW/(\d+)mW'
            }
            
            for key, pattern in power_patterns.items():
                match = re.search(pattern, line)
                if match:
                    data['power'][key] = {
                        'current': int(match.group(1)),
                        'average': int(match.group(2)),
                        'unit': 'mW'
                    }
            
            return data
            
        except Exception as e:
            logger.error(f"解析tegrastats输出失败: {e}")
            return None
    
    def get_current_status(self) -> Optional[Dict]:
        """
        获取当前系统状态
        
        Returns:
            Dict: 当前系统状态数据
        """
        # 如果没有持久进程，启动一个
        if self.process is None:
            if not self.start_tegrastats():
                # 如果启动失败，回退到单次读取
                line = self.get_single_reading()
                if line:
                    return self.parse_line(line)
                return None
        
        # 从持久进程读取数据
        try:
            if self.process and self.process.poll() is None:
                # 进程仍在运行，读取一行输出
                line = self.process.stdout.readline()
                if line and 'RAM' in line:
                    return self.parse_line(line.strip())
        except Exception as e:
            logger.error(f"从持久进程读取数据失败: {e}")
            # 重启进程
            self.stop_tegrastats()
            self.start_tegrastats()
        
        return None


# 测试函数
def test_parser():
    """测试解析器功能"""
    parser = TegrastatsParser()
    
    # 测试样例数据
    sample_line = ("10-03-2025 03:20:36 RAM 1997/62841MB (lfb 68x4MB) SWAP 0/31421MB (cached 0MB) "
                  "CPU [3%@1574,0%@1574,0%@1574,0%@1574,2%@1420,0%@1420,0%@1420,0%@1420,0%@729,0%@729,0%@729,0%@729] "
                  "GR3D_FREQ 0% cpu@45.75C soc2@43.875C soc0@43.437C tj@45.75C soc1@44.281C "
                  "VDD_GPU_SOC 2468mW/2468mW VDD_CPU_CV 246mW/246mW VIN_SYS_5V0 3383mW/3383mW")
    
    result = parser.parse_line(sample_line)
    if result:
        print("解析测试成功:")
        print(json.dumps(result, indent=2, ensure_ascii=False))
    else:
        print("解析测试失败")


if __name__ == "__main__":
    test_parser()