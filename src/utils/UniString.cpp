//
// Created by alesapin on 16.06.16.
//

#include "UniString.h"
namespace utils {
utils::UniString::UniString(const std::string& str, const std::locale& loc)
    : locale(loc) {
    using namespace boost::locale::boundary;
    data = std::make_shared<std::vector<UniCharacter>>();

    ssegment_index tmp(character, str.begin(), str.end(), locale);
    ssegment_index::iterator b, e;

    for (b = tmp.begin(), e = tmp.end(); b != e; ++b) {
        std::string s = *b;
        data->push_back(UniCharacter(s, detectTag(s, locale)));
    }
}

bool UniString::isUpperCase() const {
    for (int i = 0; i < data->size(); ++i) {
        if ((*data)[i].isalpha() && !(*data)[i].isupper())
            return false;
    }
    return !data->empty();
}

bool UniString::isLowerCase() const {
    for (int i = 0; i < data->size(); ++i) {
        if ((*data)[i].isalpha() && !(*data)[i].islower())
            return false;
    }
    return !data->empty();
}

UniString UniString::toUpperCase() const {
    UniString result;
    for (int i = 0; i < data->size(); ++i) {
        result.data->push_back((*data)[i].toUpper(this->locale));
    }
    return result;
}

UniString UniString::toLowerCase() const {
    UniString result;
    for (int i = 0; i < data->size(); ++i) {
        result.data->push_back((*data)[i].toLower(this->locale));
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const UniString& str) {
    for (int i = 0; i < str.length(); ++i) {
        os << str[i];
    }
    return os;
}

std::istream& operator>>(std::istream& is, UniString& str) {
    std::string s;
    is >> s;
    str = UniString(s, str.locale);
    return is;
}

bool UniString::operator==(const UniString& other) const {
    if (length() == other.length()) {
        for (int i = 0; i < other.data->size(); ++i) {
            if (other[i] != this->operator[](i))
                return false;
        }
        return true;
    }
    return false;
}
bool UniString::operator==(const std::string& other) const {
    std::string result;
    for (int i = 0; i < data->size(); ++i) {
        result += (*data)[i].getInnerRepr();
    }
    return result == other;
}

std::vector<UniString> UniString::split(const UniCharacter& chr) const {
    std::vector<UniString> result;
    if (isEmpty()) {
        result.push_back(*this);
        return result;
    }
    long start = 0;
    int counter = 0;
    for (int i = 0; i < data->size(); ++i) {
        if ((*data)[i] == chr) {
            result.push_back(subString(start, counter));
            start += counter + 1;
            counter = 0;
        } else {
            counter++;
        }
    }
    if (start < data->size()) {
        result.push_back(subString(start, data->size())); // до конца
    } else if (start == data->size()) {
        result.push_back(UniString("", locale));
    }
    return result;
}

std::vector<UniString> UniString::split(const UniString& str) const {
    uint start = 0;
    long fPos = -1;
    std::vector<UniString> result;
    if (data->size() == 0) {
        result.push_back(*this);
        return result;
    }
    while ((fPos = find(str, start)) != -1) {
        result.push_back(subString(start, fPos - start));
        start = fPos + str.length();
    }
    if (start < data->size()) {
        result.push_back(subString(start, data->size())); // До конца
    } else if (start == data->size()) {
        result.push_back(UniString(locale));
    }
    return result;
}

std::vector<UniString> UniString::split(char chr) const {
    UniCharacter c(std::string(1, chr), CharTag::UNKNOWN);
    return split(c);
}

long UniString::find(const UniString& other, uint start) const {
    if (other.length() + start > data->size())
        return -1;
    if (other.data->empty())
        return 0;
    long i = start, j = 0;
    long result = -1;
    while (i < data->size()) {
        if (j == other.data->size()) {
            return result;
        }
        if ((*data)[i] == other.data->operator[](j)) {
            if (result == -1) {
                result = i;
            }
            ++j;
            ++i;
        } else if (result != -1) {
            result = -1;
            j = 0;
        } else {
            ++i;
        }
    }
    if (j == other.data->size())
        return result;
    return -1;
}

long UniString::find(const UniCharacter& c, uint start) const {
    for (std::size_t i = start; i < length(); ++i) {
        if ((*data)[i] == c)
            return i;
    }
    return -1;
}

std::string UniString::getRawString(uint start) const {
    if (start > data->size()) {
        throw std::out_of_range("Required start: " + std::to_string(start) +
                                " is bigger than string length: " + std::to_string(length()));
    }
    std::string result;
    for (long i = start; i < data->size(); ++i) {
        result += (*data)[i].getInnerRepr();
    }
    return result;
}

bool UniString::operator<(const UniString& other) const {
    for (int i = 0; i < std::min(length(), other.length()); ++i) {
        if ((*data)[i] < other.data->operator[](i))
            return true;
        else if ((*data)[i] > other.data->operator[](i))
            return false;
    }
    return data->size() < other.data->size();
}

UniString UniString::operator+(const UniString& other) const {
    UniString result;
    result.data->insert(result.data->end(), this->data->begin(), this->data->end());
    result.data->insert(result.data->end(), other.data->begin(), other.data->end());
    return result;
}

UniString UniString::subString(uint start, uint len) const {
    utils::UniString result(locale);
    if (start > data->size()) {
        throw std::out_of_range("Required start: " + std::to_string(start) +
                                " is bigger than string length: " + std::to_string(length()));
    }
    if (len == -1 || start + len > data->size()) {
        len = data->size() - start;
    }
    result.data->insert(result.data->end(), data->begin() + start, data->begin() + start + len);
    return result;
}

UniString UniString::reverse() const {
    UniString result;
    result.data->insert(result.data->end(), this->data->begin(), this->data->end());
    std::reverse(result.data->begin(), result.data->end());
    return result;
}

bool UniString::endsWith(const UniString& tail) const {
    if (tail.length() > length())
        return false;
    long j = length() - 1;
    for (long i = tail.length() - 1; i >= 0; --i, --j) {
        if (tail[i] != this->operator[](static_cast<uint>(j)))
            return false;
    }
    return true;
}

bool UniString::startsWith(const UniString& head) const {
    if (head.length() > length())
        return false;
    for (std::size_t i = 0; i < head.length(); ++i) {
        if (head[i] != this->operator[](i))
            return false;
    }
    return true;
}

bool UniString::contains(const boost::u32regex& reg) const {
    return boost::u32regex_search(getRawString(), reg);
}

long UniString::find(const boost::u32regex& reg, uint start) const {
    boost::smatch results;
    if (boost::u32regex_search(getRawString(start), results, reg)) {
        return mapInnerToOuter(results.position());
    }
    return -1;
}

long UniString::mapInnerToOuter(uint index) const {
    int innerLength = 0;
    for (int i = 0; i < data->size(); ++i) {
        if (innerLength >= index)
            return i;
        innerLength += (*data)[i].getByteSize();
    }
    if (innerLength >= index)
        return data->size() - 1;
    return -1;
}

std::vector<UniString> UniString::split(const boost::u32regex& reg) const {
    std::vector<UniString> result;
    long start = 0;
    std::string innerRepr = getRawString();
    boost::u32regex_token_iterator<std::string::const_iterator>
        iter(boost::make_u32regex_token_iterator(innerRepr, reg)), end;
    for (; iter != end; ++iter) {
        long mLen = iter->second - iter->first;
        long mStart = iter->first - innerRepr.begin();
        long realStart = mapInnerToOuter(mStart);
        long realLen = mapInnerToOuter(mStart + mLen) - realStart;
        result.push_back(subString(start, realStart - start));
        start = realStart + realLen;
    }
    if (start < data->size()) {
        result.push_back(subString(start, data->size())); // До конца
    } else if (start == data->size()) {
        result.push_back(UniString(locale));
    }
    return result;
}

bool UniString::match(const boost::u32regex& reg) const {
    return boost::u32regex_match(getRawString(), reg);
}

UniString longestCommonSubstring(const UniString& a, const UniString& b) {
    if (a.isEmpty() || b.isEmpty())
        return UniString("", a.locale);
    const int a_size = a.length();
    const int b_size = b.length();

    typedef std::vector<int> solution;

    const int solution_size = b_size + 1;
    solution x(solution_size, 0), y(solution_size);

    solution* previous = &x;
    solution* current = &y;

    int max_length = 0;
    int result_index = 0;

    for (int i = a_size - 1; i >= 0; i--) {
        for (int j = b_size - 1; j >= 0; j--) {
            int& current_match = (*current)[j];
            if (a[i] != b[j]) {
                current_match = 0;
            } else {
                const int length = 1 + (*previous)[j + 1];
                if (length > max_length) {
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

UniString longestCommonSubstring(const std::vector<UniString>& strs) {
    if (strs.empty())
        throw "Empty array for longest substring";
    if (strs.size() == 1)
        return strs[0];
    UniString common = longestCommonSubstring(strs[0], strs[1]);
    for (int i = 2; i < strs.size(); ++i) {
        common = longestCommonSubstring(common, strs[i]);
    }
    return common;
}

UniString longestCommonPrefix(const std::vector<UniString>& strs) {
    if (strs.empty())
        throw "Empty array for longest substring";
    if (strs.size() == 1)
        return strs[0];
    auto itr = std::min_element(strs.begin(), strs.end(),
                                [](const UniString& f, const UniString& s) { return f.length() < s.length(); });
    std::size_t minSize = itr->length();
    int i;
    bool out = false;
    for (i = 0; i < minSize; ++i) {
        UniCharacter current = strs[0][i];
        for (int j = 1; j < strs.size(); ++j) {
            if (strs[j][i] != current) {
                out = true;
                break;
            }
        }
        if (out)
            break;
    }
    return strs[0].subString(0, i);
}

void split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

UniString UniString::replace(const UniCharacter& what, const UniCharacter& whereby) const {
    UniString result(*this);
    for (std::size_t i = 0; i < length(); ++i) {
        if ((*data)[i] == what) {
            result.data->operator[](i) = whereby;
        }
    }
    return result;
}

UniString UniString::replace(const UniString& what, const UniString& whereby) const {
    std::vector<UniString> strs = split(what);
    UniString result = strs[0];
    for (std::size_t i = 1; i < strs.size(); ++i) {
        result.data->insert(result.data->end(), whereby.data->begin(), whereby.data->end());
        result.data->insert(result.data->end(), strs[i].data->begin(), strs[i].data->end());
    }
    return result;
}
bool UniString::isNumber() const {
    for (const UniCharacter& chr : *data) {
        if (!chr.isdigit())
            return false;
    }
    return true;
}
}
