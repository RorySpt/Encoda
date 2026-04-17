#include "encoding/encoding.h"
#include "unicode.h"
#include <format>

#if defined(_WIN32)
#  define PLATFORM_WINDOWS
#  include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#  define PLATFORM_POSIX
#  include <iconv.h>
#endif

namespace Encoding
{

#if defined(PLATFORM_WINDOWS)

static std::string win_convert(std::string_view input, UINT from_cp, UINT to_cp)
{
    if (input.empty()) return {};

    // 先转到宽字符
    int wlen = MultiByteToWideChar(from_cp, 0, input.data(), static_cast<int>(input.size()), nullptr, 0);
    if (wlen <= 0)
        throw ConvertException(std::format("MultiByteToWideChar failed, error={}", GetLastError()));

    std::wstring wide(wlen, L'\0');
    MultiByteToWideChar(from_cp, 0, input.data(), static_cast<int>(input.size()), wide.data(), wlen);

    // 再转到目标编码
    int len = WideCharToMultiByte(to_cp, 0, wide.data(), wlen, nullptr, 0, nullptr, nullptr);
    if (len <= 0)
        throw ConvertException(std::format("WideCharToMultiByte failed, error={}", GetLastError()));

    std::string result(len, '\0');
    WideCharToMultiByte(to_cp, 0, wide.data(), wlen, result.data(), len, nullptr, nullptr);
    return result;
}

std::string utf8_to_gbk(std::string_view input)
{
    return win_convert(input, CP_UTF8, 936);
}

std::string gbk_to_utf8(std::string_view input)
{
    return win_convert(input, 936, CP_UTF8);
}

std::string wstring_to_utf8(std::wstring_view input)
{
    if (input.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), nullptr, 0, nullptr, nullptr);
    if (len <= 0)
        throw ConvertException(std::format("WideCharToMultiByte failed, error={}", GetLastError()));
    std::string result(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), result.data(), len, nullptr, nullptr);
    return result;
}

std::wstring utf8_to_wstring(std::string_view input)
{
    if (input.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), nullptr, 0);
    if (len <= 0)
        throw ConvertException(std::format("MultiByteToWideChar failed, error={}", GetLastError()));
    std::wstring result(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, input.data(), static_cast<int>(input.size()), result.data(), len);
    return result;
}

#elif defined(PLATFORM_POSIX)

class IconvGuard
{
public:
    explicit IconvGuard(iconv_t cd) : cd_(cd) {}
    ~IconvGuard() { if (cd_ != (iconv_t)-1) iconv_close(cd_); }
    iconv_t get() const { return cd_; }
private:
    iconv_t cd_;
};

static std::string iconv_convert(std::string_view input, const char* from, const char* to)
{
    if (input.empty()) return {};

    IconvGuard guard(iconv_open(to, from));
    if (guard.get() == (iconv_t)-1)
        throw ConvertException(std::format("iconv_open({}, {}) failed", to, from));

    std::string result;
    result.resize(input.size() * 4);

    auto* in_buf  = const_cast<char*>(input.data());
    size_t in_left = input.size();
    char*  out_buf = result.data();
    size_t out_left = result.size();

    while (in_left > 0)
    {
        size_t ret = iconv(guard.get(), &in_buf, &in_left, &out_buf, &out_left);
        if (ret == (size_t)-1)
        {
            if (errno == E2BIG)
            {
                size_t written = result.size() - out_left;
                result.resize(result.size() * 2);
                out_buf  = result.data() + written;
                out_left = result.size() - written;
            }
            else
            {
                throw ConvertException(std::format("iconv conversion failed, errno={}", errno));
            }
        }
    }

    result.resize(result.size() - out_left);
    return result;
}

std::string utf8_to_gbk(std::string_view input)  { return iconv_convert(input, "UTF-8", "GBK"); }
std::string gbk_to_utf8(std::string_view input)  { return iconv_convert(input, "GBK", "UTF-8"); }

std::string wstring_to_utf8(std::wstring_view input)
{
    // wchar_t在Linux上是UTF-32LE
    std::string_view raw{reinterpret_cast<const char*>(input.data()), input.size() * sizeof(wchar_t)};
    return iconv_convert(raw, "UTF-32LE", "UTF-8");
}

std::wstring utf8_to_wstring(std::string_view input)
{
    std::string wide = iconv_convert(input, "UTF-8", "UTF-32LE");
    std::wstring result(wide.size() / sizeof(wchar_t), L'\0');
    std::memcpy(result.data(), wide.data(), wide.size());
    return result;
}

#else
#  error "Unsupported platform"
#endif

} // namespace Encoding
