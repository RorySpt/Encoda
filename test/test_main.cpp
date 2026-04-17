#include "encoding/encoding.h"
#include <cassert>
#include <cstdio>

using namespace Encoding;

// C++20中u8""是char8_t*，需要reinterpret_cast转为char*
static std::string u8str(const char8_t* s, size_t n)
{
    return std::string(reinterpret_cast<const char*>(s), n);
}
#define U8(s) u8str(u8##s, sizeof(u8##s) - 1)

static std::string bytes(std::initializer_list<unsigned char> b)
{
    return std::string(reinterpret_cast<const char*>(b.begin()), b.size());
}

void test_utf8_utf16()
{
    std::string utf8 = U8("Hello, 世界! 🌍");
    auto utf16 = utf8_to_utf16(utf8);
    auto back  = utf16_to_utf8(utf16);
    assert(back == utf8);
    puts("PASS: utf8 <-> utf16");
}

void test_utf8_utf32()
{
    std::string utf8 = U8("Hello, 世界! 🌍");
    auto utf32 = utf8_to_utf32(utf8);
    auto back  = utf32_to_utf8(utf32);
    assert(back == utf8);
    puts("PASS: utf8 <-> utf32");
}

void test_utf16_utf32()
{
    std::string utf8 = U8("跨平台编码转换");
    auto utf16 = utf8_to_utf16(utf8);
    auto utf32 = utf16_to_utf32(utf16);
    auto back  = utf32_to_utf16(utf32);
    assert(back == utf16);
    puts("PASS: utf16 <-> utf32");
}

void test_gbk()
{
    std::string utf8 = U8("你好，世界！");
    auto gbk  = utf8_to_gbk(utf8);
    auto back = gbk_to_utf8(gbk);
    assert(back == utf8);
    puts("PASS: utf8 <-> gbk");
}

void test_wstring()
{
    std::string utf8 = U8("Hello, 世界!");
    auto wide = utf8_to_wstring(utf8);
    auto back = wstring_to_utf8(wide);
    assert(back == utf8);
    puts("PASS: utf8 <-> wstring");
}

void test_convert_generic()
{
    std::string utf8 = U8("编码转换库");
    auto gbk  = convert(utf8, Charset::utf8, Charset::gbk);
    auto back = convert(gbk,  Charset::gbk,  Charset::utf8);
    assert(back == utf8);
    puts("PASS: convert() generic interface");
}

void test_ansi()
{
    std::string utf8 = U8("你好，世界！");
    auto ansi = utf8_to_ansi(utf8);
    auto back = ansi_to_utf8(ansi);
    assert(back == utf8);
    puts("PASS: utf8 <-> ansi");
}

void test_empty()
{
    assert(utf8_to_utf16("").empty());
    assert(utf16_to_utf8(u"").empty());
    assert(utf8_to_gbk("").empty());
    puts("PASS: empty input");
}

void test_ascii()
{
    std::string ascii = "Hello, World! 0123456789";
    assert(utf16_to_utf8(utf8_to_utf16(ascii)) == ascii);
    assert(utf32_to_utf8(utf8_to_utf32(ascii)) == ascii);
    puts("PASS: ascii roundtrip");
}

// 黄金数据测试：验证中间字节序列是否正确，而非仅验证可逆性
void test_golden_utf8_utf16()
{
    // "你好" UTF-8: E4 BD A0 E5 A5 BD
    // "你好" UTF-16LE: 60 4F 7D 59
    std::string utf8 = bytes({0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD});
    std::u16string expected_utf16 = {0x4F60, 0x597D};
    assert(utf8_to_utf16(utf8) == expected_utf16);
    assert(utf16_to_utf8(expected_utf16) == utf8);
    puts("PASS: golden utf8 <-> utf16");
}

void test_golden_utf8_utf32()
{
    // U+1F30D (🌍) UTF-8: F0 9F 8C 8D
    std::string utf8 = bytes({0xF0, 0x9F, 0x8C, 0x8D});
    std::u32string expected = {0x1F30D};
    assert(utf8_to_utf32(utf8) == expected);
    assert(utf32_to_utf8(expected) == utf8);
    puts("PASS: golden utf8 <-> utf32 (surrogate pair)");
}

void test_golden_gbk()
{
    // "你好" GBK: C4 E3 BA C3
    std::string utf8 = bytes({0xE4, 0xBD, 0xA0, 0xE5, 0xA5, 0xBD});
    std::string expected_gbk = bytes({0xC4, 0xE3, 0xBA, 0xC3});
    assert(utf8_to_gbk(utf8) == expected_gbk);
    assert(gbk_to_utf8(expected_gbk) == utf8);
    puts("PASS: golden utf8 <-> gbk");
}

void test_invalid_utf8()
{
    // 非法UTF-8序列应抛出异常
    bool caught = false;
    try { utf8_to_utf16(bytes({0xFF, 0xFE})); }
    catch (const ConvertException&) { caught = true; }
    assert(caught);
    puts("PASS: invalid utf8 throws");
}

int main()
{
    test_utf8_utf16();
    test_utf8_utf32();
    test_utf16_utf32();
    test_gbk();
    test_wstring();
    test_convert_generic();
    test_ansi();
    test_empty();
    test_ascii();
    test_golden_utf8_utf16();
    test_golden_utf8_utf32();
    test_golden_gbk();
    test_invalid_utf8();
    puts("All tests passed.");
    return 0;
}
