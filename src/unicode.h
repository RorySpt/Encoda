#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include "encoding/encoding.h"

namespace Encoding::detail
{

// 从UTF-8字节流解码一个码点，返回消耗的字节数，失败返回0
inline uint32_t decode_utf8(const char* src, size_t len, char32_t& cp)
{
    const auto* s = reinterpret_cast<const uint8_t*>(src);
    if (len == 0) return 0;

    if (s[0] < 0x80)
    {
        cp = s[0];
        return 1;
    }
    if ((s[0] & 0xE0) == 0xC0 && len >= 2 && (s[1] & 0xC0) == 0x80)
    {
        cp = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        return cp >= 0x80 ? 2 : 0;
    }
    if ((s[0] & 0xF0) == 0xE0 && len >= 3 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80)
    {
        cp = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        return cp >= 0x800 && (cp < 0xD800 || cp > 0xDFFF) ? 3 : 0;
    }
    if ((s[0] & 0xF8) == 0xF0 && len >= 4 && (s[1] & 0xC0) == 0x80 && (s[2] & 0xC0) == 0x80 && (s[3] & 0xC0) == 0x80)
    {
        cp = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        return cp >= 0x10000 && cp <= 0x10FFFF ? 4 : 0;
    }
    return 0;
}

// 将码点编码为UTF-8，返回写入字节数
inline uint32_t encode_utf8(char32_t cp, char* dst)
{
    auto* d = reinterpret_cast<uint8_t*>(dst);
    if (cp < 0x80)        { d[0] = cp; return 1; }
    if (cp < 0x800)       { d[0] = 0xC0 | (cp >> 6); d[1] = 0x80 | (cp & 0x3F); return 2; }
    if (cp < 0x10000)     { d[0] = 0xE0 | (cp >> 12); d[1] = 0x80 | ((cp >> 6) & 0x3F); d[2] = 0x80 | (cp & 0x3F); return 3; }
    if (cp <= 0x10FFFF)   { d[0] = 0xF0 | (cp >> 18); d[1] = 0x80 | ((cp >> 12) & 0x3F); d[2] = 0x80 | ((cp >> 6) & 0x3F); d[3] = 0x80 | (cp & 0x3F); return 4; }
    return 0;
}

// UTF-32码点 -> UTF-16 surrogate pair（或单个单元）
inline uint32_t encode_utf16(char32_t cp, char16_t* dst)
{
    if (cp < 0x10000) { dst[0] = static_cast<char16_t>(cp); return 1; }
    cp -= 0x10000;
    dst[0] = static_cast<char16_t>(0xD800 | (cp >> 10));
    dst[1] = static_cast<char16_t>(0xDC00 | (cp & 0x3FF));
    return 2;
}

// UTF-16 -> UTF-32码点，返回消耗的char16_t数量
inline uint32_t decode_utf16(const char16_t* src, size_t len, char32_t& cp)
{
    if (len == 0) return 0;
    if (src[0] >= 0xD800 && src[0] <= 0xDBFF)
    {
        if (len < 2 || src[1] < 0xDC00 || src[1] > 0xDFFF) return 0;
        cp = 0x10000 + ((static_cast<char32_t>(src[0] - 0xD800) << 10) | (src[1] - 0xDC00));
        return 2;
    }
    cp = src[0];
    return 1;
}

} // namespace Encoding::detail
