# HC32L19x GCC 编译指南

## 概述

本目录提供 HC32L19x 系列 MCU 的 GCC 编译配置，适用于使用 GNU Arm Embedded Toolchain 的开发者。

## 文件结构

```
GCC/
├── startup_hc32l19x.S    # 启动文件 (中断向量表、复位处理程序)
├── hc32l19x.ld           # 链接脚本 (内存布局定义)
├── hc32l19x.cfg          # OpenOCD 配置文件 (调试/下载)
├── Makefile              # 构建脚本
└── README.md             # 本文件
```

## 环境要求

### 1. 安装 GNU Arm Embedded Toolchain

**Windows:**
- 下载：https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
- 安装后添加工具链到 PATH 环境变量

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install gcc-arm-none-eabi
```

**macOS:**
```bash
brew install arm-none-eabi-gcc
```

### 2. 安装 OpenOCD (可选，用于烧录)

**Windows:**
- 下载：http://openocd.org/pages/getting-openocd.html

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install openocd
```

**macOS:**
```bash
brew install openocd
```

## 使用方法

### 基本编译

```bash
# 设置项目根目录环境变量 (Windows PowerShell)
$env:PROJECT_ROOT="E:\github_download\_ZeroZap\Maker\xhsc\HC32L19x"

# 设置项目根目录环境变量 (Linux/macOS)
export PROJECT_ROOT=/path/to/HC32L19x

# 编译模板示例
make EXAMPLE=template

# 编译 GPIO 输出示例
make EXAMPLE=gpio/gpio_output
```

### 可用目标

| 目标 | 说明 |
|------|------|
| `all` | 编译 ELF、HEX 和 BIN 文件 (默认) |
| `clean` | 清除构建文件 |
| `flash` | 使用 OpenOCD 烧录到目标板 |
| `debug` | 启动 GDB 调试会话 |
| `help` | 显示帮助信息 |

### 编译示例

```bash
# 编译 GPIO 输出示例
make EXAMPLE=gpio/gpio_output

# 编译并烧录
make EXAMPLE=gpio/gpio_output flash

# 编译并调试
make EXAMPLE=gpio/gpio_output debug

# 清理并重新编译
make EXAMPLE=uart/uart_poll clean all
```

## 输出文件

编译成功后，`build/` 目录将生成以下文件：

- `project.elf` - ELF 格式的可执行文件 (用于调试)
- `project.hex` - Intel HEX 格式 (用于烧录)
- `project.bin` - 二进制格式 (用于烧录)

## 内存布局

HC32L19x 内存配置：

| 存储器 | 起始地址 | 大小 |
|--------|----------|------|
| FLASH  | 0x00000000 | 256 KB |
| SRAM   | 0x20000000 | 16 KB  |

**注意:** 中断向量表被重定位到 SRAM 以获得更快的中断响应。

## 烧录方法

### 使用 OpenOCD

```bash
make EXAMPLE=gpio/gpio_output flash
```

### 手动烧录

```bash
openocd -f interface/cmsis-dap.cfg -f hc32l19x.cfg -c "program build/project.hex verify reset exit"
```

### 使用其他烧录工具

如果使用其他烧录工具，请使用生成的 `.hex` 或 `.bin` 文件。

## 调试方法

### 使用 GDB

```bash
# 启动 GDB
make EXAMPLE=gpio/gpio_output debug

# 或手动启动
arm-none-eabi-gdb build/project.elf
```

### GDB 常用命令

```gdb
(gdb) target remote :3333      # 连接到 OpenOCD
(gdb) monitor reset halt       # 复位并暂停
(gdb) load                     # 下载程序
(gdb) continue                 # 继续运行
(gdb) next                     # 单步执行
(gdb) break main               # 在 main 函数设置断点
(gdb) info registers           # 查看寄存器状态
```

## 自定义配置

### 修改优化级别

编辑 `Makefile`:

```makefile
# 优化选项
CFLAGS += -O0    # 无优化 (调试用)
# CFLAGS += -O1  # 优化代码大小
# CFLAGS += -O2  # 优化执行速度
# CFLAGS += -Os  # 优化代码大小 (推荐发布版本)
```

### 修改栈和堆大小

编辑 `hc32l19x.ld`:

```ld
_Min_Stack_Size = 0x400;  /* 1024 bytes */
_Min_Heap_Size  = 0x400;  /* 1024 bytes */
```

### 添加新的源文件

在 `Makefile` 中添加：

```makefile
C_SOURCES += /path/to/your/source.c
```

## 常见问题

### 1. `make: command not found`

**Windows:** 安装 Make 工具 (如 MinGW 或 Cygwin)

**Linux/macOS:** Make 通常预装

### 2. `arm-none-eabi-gcc: command not found`

确保 GNU Arm Embedded Toolchain 已正确安装并添加到 PATH 环境变量。

### 3. 烧录失败

- 检查调试器连接
- 确认 OpenOCD 配置正确
- 尝试降低 `adapter_khz` 速度

### 4. 程序不运行

- 检查 `SystemInit()` 是否被调用
- 确认时钟配置正确
- 检查中断向量表位置

## 参考资源

- [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- [OpenOCD Documentation](http://openocd.org/doc/)
- [HC32L19x 数据手册](https://www.xhsc.com.cn/)
- [ARM Cortex-M0+ 技术参考手册](https://developer.arm.com/documentation/)

## 许可证

BSD 3-Clause License
