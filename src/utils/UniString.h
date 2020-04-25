#pragma once

#include <vector>
#include "UniCharFuncs.h"
#include <boost/locale.hpp>
#include <locale>
#include <iostream>
#include <sstream>
#include <unicode/unistr.h>

namespace utils {


/**
 * Класс представляющий
 * юникодную строку
 * с возможностью наложения
 * регулярок (тоже юникодных)
 */
class UniString {


private:
    icu::UnicodeString data;

public:
    UniString() = default;
    explicit UniString(const std::string& str);

    UniString(const UniString& other)
        : data(other.data)
    {
    }

    UniString(UniString&& other)
        : data(std::move(other.data))
    {
    }

    UniString& operator=(const UniString& other) {
        data = other.data;
        return *this;
    }

    UniString& operator=(UniString&& other) {
        data = std::move(other.data);
        return *this;
    }


    char16_t operator[](size_t i) const
    {
        return data[i];
    }

    char16_t charAt(size_t i) const
    {
        return data[i];
    }

    size_t length() const {
        return data.length();
    }

    bool isEmpty() const {
        return data.isEmpty();
    }

    bool operator==(const UniString& other) const;

    bool operator!=(const UniString& other) const {
        return !(*this == other);
    }

    bool operator==(const std::string& other) const;

    bool operator!=(const std::string& other) const {
        return !(*this == other);
    }

    bool isUpperCase() const;

    bool isLowerCase() const;

    UniString toUpperCase() const;

    UniString toLowerCase() const;

    long find(const UniString& other, size_t start = 0) const;
    long find(char16_t c, size_t start = 0) const;
    std::vector<UniString> split(char16_t chr) const;

    std::vector<UniString> split(const UniString& str) const;

    std::vector<UniString> split(char chr) const;

    std::string getRawString(size_t start = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const UniString& str);
    friend std::istream& operator>>(std::istream& is, UniString& str);

    /**
     * Лексикографическое сравнение строк
     */
    bool operator<(const UniString& other) const;

    bool operator>(const UniString& other) const {
        return !(*this < other) && !(*this == other);
    }

    UniString operator+(const UniString& other) const;

    UniString subString(size_t start = 0, size_t len = std::string::npos) const;

    UniString cut(size_t len) const {
        return subString(0, len);
    }

    UniString rcut(size_t len) const {
        if (len >= length())
            return *this;
        return subString(length() - len);
    }
    UniString reverse() const;

    bool endsWith(const UniString& tail) const;

    bool startsWith(const UniString& head) const;


    inline bool startsWith(char16_t head) const {
        if (data.isEmpty())
            return false;
        return charAt(0) == head;
    }
    bool contains(const UniString& str) const {
        return find(str) != -1;
    }

    bool contains(const char16_t& c) const {
        return find(c) != -1;
    }

    UniString replace(const UniString& what, const UniString& whereby) const;

    UniString replace(char16_t what, char16_t whereby) const;

    bool isNumber() const;

    friend UniString longestCommonSubstring(const UniString& first, const UniString& second);

    friend UniString longestCommonSubstring(const std::vector<UniString>& strs);

    friend UniString longestCommonPrefix(const std::vector<UniString>& strs);
};
void split(const std::string& s, char delim, std::vector<std::string>& elements);

inline bool isNumber(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
}
// custom specialization of std::hash can be injected in namespace std
namespace std {
template <>
struct hash<utils::UniString> {
    typedef utils::UniString argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const {
        result_type h1{};
        for (size_t i = 0; i < s.length(); ++i) {
            h1 += std::hash<char16_t>{}(s.charAt(i));
        }
        return h1; // or use boost::hash_combine
    }
};

}
