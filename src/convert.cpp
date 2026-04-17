#include "encoding/encoding.h"
#include <format>

namespace Encoding
{

std::string convert(std::string_view input, Charset from, Charset to)
{
    if (from == to) return std::string(input);

    // 先统一转到UTF-8
    std::string utf8;
    switch (from)
    {
    case Charset::utf8:
        utf8 = std::string(input);
        break;
    case Charset::utf16_le:
    {
        if (input.size() % 2 != 0)
            throw ConvertException("UTF-16 input size must be even");
        std::u16string_view sv{reinterpret_cast<const char16_t*>(input.data()), input.size() / 2};
        utf8 = utf16_to_utf8(sv);
        break;
    }
    case Charset::utf32_le:
    {
        if (input.size() % 4 != 0)
            throw ConvertException("UTF-32 input size must be a multiple of 4");
        std::u32string_view sv{reinterpret_cast<const char32_t*>(input.data()), input.size() / 4};
        utf8 = utf32_to_utf8(sv);
        break;
    }
    case Charset::gbk:
        utf8 = gbk_to_utf8(input);
        break;
    default:
        throw ConvertException(std::format("Unsupported source charset: {}", static_cast<int>(from)));
    }

    // 再从UTF-8转到目标编码
    switch (to)
    {
    case Charset::utf8:
        return utf8;
    case Charset::utf16_le:
    {
        auto u16 = utf8_to_utf16(utf8);
        return std::string{reinterpret_cast<const char*>(u16.data()), u16.size() * 2};
    }
    case Charset::utf32_le:
    {
        auto u32 = utf8_to_utf32(utf8);
        return std::string{reinterpret_cast<const char*>(u32.data()), u32.size() * 4};
    }
    case Charset::gbk:
        return utf8_to_gbk(utf8);
    default:
        throw ConvertException(std::format("Unsupported target charset: {}", static_cast<int>(to)));
    }
}

} // namespace Encoding
