#pragma once

#include <algorithm>
#include <sstream>
#include <vector>
#include <unicode/unistr.h>
#include <xmorphy/utils/UniCharFuncs.h>

namespace X
{

class UniString
{
private:
    icu::UnicodeString data;
    mutable std::string raw_string;
    size_t symbols_length{0};

public:
    UniString() = default;
    explicit UniString(const std::string & str);
    explicit UniString(const char * str);

    UniString(const char * begin, const char * end);
    explicit UniString(char16_t ch)
        : data(ch)
        , symbols_length(1) {}

    UniString(const UniString & other)
        : data(other.data)
        , raw_string(other.raw_string)
        , symbols_length(other.symbols_length)
    {}

    UniString(UniString && other)
        : data(std::move(other.data))
        , raw_string(std::move(other.raw_string))
        , symbols_length(other.symbols_length)
    {}

    UniString & operator=(const UniString & other)
    {
        data = other.data;
        raw_string = other.raw_string;
        symbols_length = other.symbols_length;
        return *this;
    }

    UniString & operator=(UniString && other)
    {
        data = std::move(other.data);
        raw_string = std::move(other.raw_string);
        symbols_length = other.symbols_length;
        return *this;
    }

    char16_t operator[](size_t i) const { return data[i]; }

    char16_t charAt(size_t i) const { return data[i]; }
    std::string charAtAsString(size_t i) const;

    size_t length() const { return symbols_length; }
    /// quite stupid heuristic
    size_t byteLength() const { return length() * 2; }

    bool isEmpty() const { return symbols_length == 0; }

    bool operator==(const UniString & other) const;

    bool operator!=(const UniString & other) const { return !(*this == other); }

    bool operator==(const std::string & other) const;

    bool operator!=(const std::string & other) const { return !(*this == other); }

    bool isUpperCase() const;

    bool isLowerCase() const;

    UniString toUpperCase() const;

    UniString toLowerCase() const;

    void toUpperCaseInPlace();

    void toLowerCaseInPlace();

    size_t find(const UniString & other, size_t start = 0) const;
    size_t find(char16_t c, size_t start = 0) const;
    std::vector<UniString> split(char16_t chr) const;

    std::vector<UniString> split(const UniString & str) const;

    std::vector<UniString> split(char chr) const;

    const std::string & getRawString() const;

    friend std::ostream & operator<<(std::ostream & os, const UniString & str);
    friend std::istream & operator>>(std::istream & is, UniString & str);

    bool operator<(const UniString & other) const;

    bool operator>(const UniString & other) const { return !(*this < other) && !(*this == other); }

    UniString operator+(const UniString & other) const;

    UniString subString(size_t start = 0, size_t len = std::string::npos) const;

    UniString tempSubString(size_t start = 0, size_t len = std::string::npos) const;

    UniString cut(size_t len) const { return subString(0, len); }

    UniString rcut(size_t len) const
    {
        if (len >= length())
            return *this;
        return subString(length() - len);
    }
    UniString reverse() const;

    bool endsWith(const UniString & tail) const;

    bool startsWith(const UniString & head) const;

    inline bool startsWith(char16_t head) const
    {
        if (isEmpty())
            return false;
        return charAt(0) == head;
    }
    bool contains(const UniString & str) const { return find(str) != std::string::npos; }

    bool contains(const char16_t & c) const { return find(c) != std::string::npos; }

    void replaceInPlace(size_t pos, char16_t symbol);

    UniString replace(const UniString & what, const UniString & whereby) const;

    UniString replace(char16_t what, char16_t whereby) const;

    bool isNumber() const;

    friend UniString longestCommonSubstring(const UniString & first, const UniString & second);

    friend UniString longestCommonSubstring(const std::vector<UniString> & strs);

    friend UniString longestCommonPrefix(const std::vector<UniString> & strs);
};
void split(const std::string & s, char delim, std::vector<std::string> & elements);

inline bool isNumber(const std::string & s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
}
// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<X::UniString>
{
    typedef X::UniString argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const & s) const
    {
        result_type h1{};
        for (size_t i = 0; i < s.length(); ++i)
        {
            h1 += std::hash<char16_t>{}(s.charAt(i));
        }
        return h1; // or use boost::hash_combine
    }
};

}
