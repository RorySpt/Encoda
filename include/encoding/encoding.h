#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <span>
#include <cstdint>

namespace Encoding
{

// 编码类型
enum class Charset
{
    utf8,
    utf16_le,
    utf16_be,
    utf32_le,
    utf32_be,
    gbk,
    ansi,  // 系统当前代码页
};

// 编码转换异常
class ConvertException : public std::runtime_error
{
public:
    explicit ConvertException(const std::string& msg) : std::runtime_error(msg) {}
};

// UTF-8 <-> UTF-16
std::u16string utf8_to_utf16(std::string_view input);
std::string    utf16_to_utf8(std::u16string_view input);
std::u16string utf8_to_utf16be(std::string_view input);
std::string    utf16be_to_utf8(std::u16string_view input);

// UTF-8 <-> UTF-32
std::u32string utf8_to_utf32(std::string_view input);
std::string    utf32_to_utf8(std::u32string_view input);
std::u32string utf8_to_utf32be(std::string_view input);
std::string    utf32be_to_utf8(std::u32string_view input);

// UTF-16 <-> UTF-32
std::u32string utf16_to_utf32(std::u16string_view input);
std::u16string utf32_to_utf16(std::u32string_view input);

// UTF-8 <-> GBK
std::string utf8_to_gbk(std::string_view input);
std::string gbk_to_utf8(std::string_view input);

// UTF-8 <-> 系统当前 ANSI 代码页（Windows: CP_ACP，POSIX: 当前 locale）
std::string utf8_to_ansi(std::string_view input);
std::string ansi_to_utf8(std::string_view input);

// 宽字符（平台相关）<-> UTF-8
std::string  wstring_to_utf8(std::wstring_view input);
std::wstring utf8_to_wstring(std::string_view input);

// 通用转换接口
std::string convert(std::string_view input, Charset from, Charset to);

// 按 code page 编号转换（Windows CP 编号，如 932=Shift-JIS, 949=EUC-KR）
std::string convert_cp(std::string_view input, uint32_t from_cp, uint32_t to_cp);

} // namespace Encoding
