#include <xmorphy/utils/UniString.h>
#include <codecvt>
#include <exception>
#include <locale>
#include <unicode/unistr.h>
#include <cstring>

namespace X
{
UniString::UniString(const char * str)
    : data(icu::UnicodeString::fromUTF8(icu::StringPiece(str, std::strlen(str))))
    , raw_string(str, std::strlen(str))
    , symbols_length(data.length())
{
}

UniString::UniString(const char * begin, const char * end)
    : data(icu::UnicodeString::fromUTF8(icu::StringPiece(begin, end - begin)))
    , raw_string(begin, end)
    , symbols_length(data.length())
{
}
UniString::UniString(const std::string & str)
    : data(icu::UnicodeString::fromUTF8(icu::StringPiece(str.data(), str.length())))
    , raw_string(str)
    , symbols_length(data.length())
{
}

bool UniString::isUpperCase() const
{
    for (size_t i = 0; i < length(); ++i)
    {
        if (X::isalpha(data[i]) && !X::isupper(data[i]))
            return false;
    }
    return !isEmpty();
}

bool UniString::isLowerCase() const
{
    for (size_t i = 0; i < length(); ++i)
    {
        if (X::isalpha(data[i]) && !X::islower(data[i]))
            return false;
    }
    return !isEmpty();
}

UniString UniString::toUpperCase() const
{
    UniString result;
    result.data = data;
    result.symbols_length = symbols_length;
    result.toUpperCaseInPlace();
    return result;
}

UniString UniString::toLowerCase() const
{
    UniString result;
    result.data = data;
    result.symbols_length = symbols_length;
    result.toLowerCaseInPlace();
    return result;

}

void UniString::toUpperCaseInPlace()
{
    data.toUpper();
    raw_string.clear();
    data.toUTF8String(raw_string);
}

void UniString::toLowerCaseInPlace()
{
    data.toLower();
    raw_string.clear();
    data.toUTF8String(raw_string);
}


std::ostream & operator<<(std::ostream & os, const UniString & str)
{
    std::string result;
    str.data.toUTF8String(result);
    os << result;
    return os;
}

std::istream & operator>>(std::istream & is, UniString & str)
{
    std::string s;
    is >> s;
    str = UniString(s);
    return is;
}

bool UniString::operator==(const UniString & other) const
{
    return symbols_length == other.symbols_length && data == other.data;
}
bool UniString::operator==(const std::string & other) const
{
    return data == UniString(other).data;
}

std::vector<UniString> UniString::split(char chr) const
{
    return split(char16_t(chr));
}


std::vector<UniString> UniString::split(char16_t chr) const
{
    std::vector<UniString> result;
    if (isEmpty())
    {
        result.push_back(*this);
        return result;
    }
    size_t start = 0;
    int counter = 0;
    for (size_t i = 0; i < length(); ++i)
    {
        if (data[i] == chr)
        {
            result.push_back(subString(start, counter));
            start += counter + 1;
            counter = 0;
        }
        else
        {
            counter++;
        }
    }
    if (start < length())
    {
        result.push_back(subString(start, length())); // до конца
    }
    else if (start == length())
    {
        result.push_back(UniString(""));
    }
    return result;
}

std::vector<UniString> UniString::split(const UniString & str) const
{
    size_t start = 0;
    long fPos = -1;
    std::vector<UniString> result;
    if (data.length() == 0)
    {
        result.push_back(*this);
        return result;
    }
    while ((fPos = find(str, start)) != -1)
    {
        result.push_back(subString(start, fPos - start));
        start = fPos + str.length();
    }
    if (start < length())
    {
        result.push_back(subString(start, length())); // До конца
    }
    else if (start == length())
    {
        result.emplace_back();
    }
    return result;
}

size_t UniString::find(const UniString & other, size_t start) const
{
    if (start > length())
        return std::string::npos;
    if (other.isEmpty())
        return 0;
    return data.indexOf(other.data, start);
}

size_t UniString::find(char16_t c, size_t start) const
{
    return data.indexOf(c, start);
}

const std::string & UniString::getRawString() const
{
    if (length() != 0 && raw_string.empty())
        data.toUTF8String(raw_string);

    return raw_string;
}

bool UniString::operator<(const UniString & other) const
{
    return data < other.data;
}

UniString UniString::operator+(const UniString & other) const
{
    UniString result;
    result.data = data + other.data;
    result.symbols_length = result.data.length();
    return result;
}

UniString UniString::subString(size_t start, size_t len) const
{
    if (start > length())
        throw std::out_of_range("Required start: " + std::to_string(start) + " is bigger than string length: " + std::to_string(length()));

    if (len == std::string::npos)
        len = length() - start;

    UniString result;
    result.data.append(data, start, len);
    result.symbols_length = result.data.length();
    return result;
}

UniString UniString::tempSubString(size_t start, size_t len) const
{
    if (start > length())
        throw std::out_of_range("Required start: " + std::to_string(start) + " is bigger than string length: " + std::to_string(length()));

    if (len == std::string::npos)
        len = length() - start;

    UniString result;
    result.data.fastCopyFrom(data.tempSubString(start, len));
    result.symbols_length = result.data.length();
    return result;
}

UniString UniString::reverse() const
{
    UniString result;
    result.data = data;
    result.data.reverse();
    result.symbols_length = symbols_length;
    return result;
}

bool UniString::endsWith(const UniString & tail) const
{
    return data.endsWith(tail.data);
}

bool UniString::startsWith(const UniString & head) const
{
    return data.startsWith(head.data);
}

UniString longestCommonSubstring(const UniString & a, const UniString & b)
{
    if (a.isEmpty() || b.isEmpty())
        return UniString("");
    const size_t a_size = a.length();
    const size_t b_size = b.length();

    using Result = std::vector<size_t>;

    const int solution_size = b_size + 1;
    Result x(solution_size, 0);
    Result y(solution_size);

    Result * previous = &x;
    Result * current = &y;

    int max_length = 0;
    int result_index = 0;

    for (long i = a_size - 1; i >= 0; i--)
    {
        for (long j = b_size - 1; j >= 0; j--)
        {
            size_t & current_match = (*current)[j];
            if (a.charAt(i) != b.charAt(j))
            {
                current_match = 0;
            }
            else
            {
                const int length = 1 + (*previous)[j + 1];
                if (length > max_length)
                {
                    max_length = length;
                    result_index = i;
                }

                current_match = length;
            }
        }

        std::swap(previous, current);
    }
    return a.subString(result_index, max_length);
}

UniString longestCommonSubstring(const std::vector<UniString> & strs)
{
    if (strs.empty())
        throw std::runtime_error("Empty array for longest substring");
    if (strs.size() == 1)
        return strs[0];
    UniString common = longestCommonSubstring(strs[0], strs[1]);
    for (size_t i = 2; i < strs.size(); ++i)
    {
        common = longestCommonSubstring(common, strs[i]);
    }
    return common;
}

UniString longestCommonPrefix(const std::vector<UniString> & strs)
{
    if (strs.empty())
        throw std::runtime_error("Empty array for longest substring");
    if (strs.size() == 1)
        return strs[0];
    auto itr = std::min_element(strs.begin(), strs.end(), [](const UniString & f, const UniString & s) { return f.length() < s.length(); });
    std::size_t minSize = itr->length();
    size_t i;
    bool out = false;
    for (i = 0; i < minSize; ++i)
    {
        auto current = strs[0].charAt(i);
        for (size_t j = 1; j < strs.size(); ++j)
        {
            if (strs[j].charAt(i) != current)
            {
                out = true;
                break;
            }
        }
        if (out)
            break;
    }
    return strs[0].subString(0, i);
}

void split(const std::string & s, char delim, std::vector<std::string> & elems)
{
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        elems.push_back(item);
    }
}


void UniString::replaceInPlace(size_t pos, char16_t symbol)
{
    data.replace(pos, 1, symbol);
}

UniString UniString::replace(char16_t what, char16_t whereby) const
{
    UniString result;
    result.data = data;
    result.data.findAndReplace(icu::UnicodeString(what), icu::UnicodeString(whereby));
    result.symbols_length = symbols_length;
    return result;
}

UniString UniString::replace(const UniString & what, const UniString & whereby) const
{
    UniString result;
    result.data = data;
    result.data.findAndReplace(what.data, whereby.data);
    result.symbols_length = result.data.length();
    return result;
}

bool UniString::isNumber() const
{
    for (size_t i = 0; i < length(); ++i)
    {
        if (!X::isdigit(data[i]))
            return false;
    }
    return true;
}

std::string UniString::charAtAsString(size_t i) const
{
    static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> UTF16CONV;
    return UTF16CONV.to_bytes(charAt(i));
}
}
