#include "encoding/encoding.h"
#include "unicode.h"
#include <format>

namespace Encoding
{

std::u16string utf8_to_utf16(std::string_view input)
{
    std::u16string result;
    result.reserve(input.size());
    const char* src = input.data();
    size_t remaining = input.size();

    while (remaining > 0)
    {
        char32_t cp{};
        uint32_t consumed = detail::decode_utf8(src, remaining, cp);
        if (consumed == 0)
            throw ConvertException(std::format("Invalid UTF-8 sequence at offset {}", input.size() - remaining));

        char16_t units[2]{};
        uint32_t written = detail::encode_utf16(cp, units);
        result.append(units, written);

        src += consumed;
        remaining -= consumed;
    }
    return result;
}

std::string utf16_to_utf8(std::u16string_view input)
{
    std::string result;
    result.reserve(input.size() * 3);
    const char16_t* src = input.data();
    size_t remaining = input.size();

    while (remaining > 0)
    {
        char32_t cp{};
        uint32_t consumed = detail::decode_utf16(src, remaining, cp);
        if (consumed == 0)
            throw ConvertException(std::format("Invalid UTF-16 sequence at offset {}", input.size() - remaining));

        char buf[4]{};
        uint32_t written = detail::encode_utf8(cp, buf);
        result.append(buf, written);

        src += consumed;
        remaining -= consumed;
    }
    return result;
}

std::u32string utf8_to_utf32(std::string_view input)
{
    std::u32string result;
    result.reserve(input.size());
    const char* src = input.data();
    size_t remaining = input.size();

    while (remaining > 0)
    {
        char32_t cp{};
        uint32_t consumed = detail::decode_utf8(src, remaining, cp);
        if (consumed == 0)
            throw ConvertException(std::format("Invalid UTF-8 sequence at offset {}", input.size() - remaining));
        result.push_back(cp);
        src += consumed;
        remaining -= consumed;
    }
    return result;
}

std::string utf32_to_utf8(std::u32string_view input)
{
    std::string result;
    result.reserve(input.size() * 4);
    for (char32_t cp : input)
    {
        char buf[4]{};
        uint32_t written = detail::encode_utf8(cp, buf);
        if (written == 0)
            throw ConvertException(std::format("Invalid codepoint U+{:04X}", static_cast<uint32_t>(cp)));
        result.append(buf, written);
    }
    return result;
}

std::u32string utf16_to_utf32(std::u16string_view input)
{
    std::u32string result;
    result.reserve(input.size());
    const char16_t* src = input.data();
    size_t remaining = input.size();
    while (remaining > 0)
    {
        char32_t cp{};
        uint32_t consumed = detail::decode_utf16(src, remaining, cp);
        if (consumed == 0)
            throw ConvertException(std::format("Invalid UTF-16 sequence at offset {}", input.size() - remaining));
        result.push_back(cp);
        src += consumed;
        remaining -= consumed;
    }
    return result;
}

std::u16string utf32_to_utf16(std::u32string_view input)
{
    std::u16string result;
    result.reserve(input.size());
    for (char32_t cp : input)
    {
        char16_t units[2]{};
        uint32_t written = detail::encode_utf16(cp, units);
        if (written == 0)
            throw ConvertException(std::format("Invalid codepoint U+{:04X}", static_cast<uint32_t>(cp)));
        result.append(units, written);
    }
    return result;
}

// BE 辅助：翻转字节序
static std::u16string swap16(std::u16string s)
{
    for (auto& c : s) c = (c >> 8) | (c << 8);
    return s;
}

static std::u32string swap32(std::u32string s)
{
    for (auto& c : s)
        c = ((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) | ((c & 0xFF000000) >> 24);
    return s;
}

std::u16string utf8_to_utf16be(std::string_view input) { return swap16(utf8_to_utf16(input)); }
std::string    utf16be_to_utf8(std::u16string_view input) { return utf16_to_utf8(swap16(std::u16string(input))); }

std::u32string utf8_to_utf32be(std::string_view input) { return swap32(utf8_to_utf32(input)); }
std::string    utf32be_to_utf8(std::u32string_view input) { return utf32_to_utf8(swap32(std::u32string(input))); }

} // namespace Encoding
