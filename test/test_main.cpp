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

int main()
{
    test_utf8_utf16();
    test_utf8_utf32();
    test_utf16_utf32();
    test_gbk();
    test_wstring();
    test_convert_generic();
    test_empty();
    test_ascii();
    puts("All tests passed.");
    return 0;
}
