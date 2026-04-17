# Encoda

跨平台 C++20 编码转换库，零第三方依赖（GBK/ANSI 在 Windows 上使用 WinAPI，Linux/macOS 使用 iconv）。

## 支持的编码

| 编码 | 说明 |
|------|------|
| UTF-8 | |
| UTF-16 LE | |
| UTF-32 LE | |
| GBK | CP936 |
| ANSI | 系统当前代码页（中文 Windows = GBK，日文 = Shift-JIS 等） |

## 构建

需要 CMake 3.20+，C++20 编译器（MSVC 2022 / GCC 12+ / Clang 14+）。

```bash
cmake -B build
cmake --build build
```

运行测试：

```bash
cmake -B build -DENCODING_BUILD_TESTS=ON
cmake --build build
./build/Debug/encoding_test   # Windows
./build/encoding_test         # Linux/macOS
```

## 用法

```cpp
#include "encoding/encoding.h"
using namespace Encoding;

// 直接转换
std::u16string utf16 = utf8_to_utf16("你好");
std::string    utf8  = utf16_to_utf8(utf16);

std::string gbk  = utf8_to_gbk("你好");
std::string ansi = utf8_to_ansi("你好");  // 自动适配系统代码页

// 通用接口
std::string result = convert(input, Charset::gbk, Charset::utf8);
```

## 文件结构

```
include/encoding/encoding.h   # 公共 API
src/unicode.h                 # UTF 编解码底层（内部）
src/utf_convert.cpp           # UTF-8/16/32 互转（纯 C++20，无依赖）
src/platform_convert.cpp      # GBK/ANSI/wstring（Windows WinAPI / POSIX iconv）
src/convert.cpp               # 通用 convert() 接口
```
