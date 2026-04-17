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
    return utf8_to_utf32(utf16_to_utf8(input));
}

std::u16string utf32_to_utf16(std::u32string_view input)
{
    return utf8_to_utf16(utf32_to_utf8(input));
}

} // namespace Encoding
