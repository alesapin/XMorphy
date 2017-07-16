//
// Created by alesapin on 16.06.16.
//

#ifndef CROSSTOKENIZER_UNISTRING_H
#define CROSSTOKENIZER_UNISTRING_H

#include <vector>
#include "UniCharacter.h"
#include <boost/locale.hpp>
#include <locale>
#include <iostream>
#include <sstream>
#include <boost/regex/icu.hpp>
#include "../base/IEntity.h"

namespace utils {

typedef std::size_t uint;

/**
 * Класс представляющий
 * юникодную строку
 * с возможностью наложения
 * регулярок (тоже юникодных)
 */
class UniString : public base::IEntity {
private:
    std::shared_ptr<std::vector<UniCharacter>> data;
    std::locale locale;

    /**
     * Преобразует индекс
     * из внутреннего представления во внешнее
     */
    long mapInnerToOuter(uint index) const;

public:
    typedef std::vector<UniCharacter>::const_iterator iterator;

    /**
     * Итераторы на внутреннее представление
     */
    iterator begin() const {
        return data->begin();
    }

    iterator end() const {
        return data->end();
    }

    explicit UniString(const std::string& str, const std::locale& loc = std::locale());

    explicit UniString(const std::locale& loc = std::locale())
        : locale(loc){
        data = std::make_shared<std::vector<UniCharacter>>();
    };

    /**
     * Конструктор копий
     */
    UniString(const UniString& other)
        : data(other.data)
        , locale(other.locale) {
    }

    /**
     * Конструктор мува
     */
    UniString(UniString&& other)
        : data(std::move(other.data))
        , locale(std::move(other.locale)) {
    }

    /**
     * Operator =
     */
    UniString& operator=(const UniString& other) {
        data = other.data;
        locale = other.locale;
        return *this;
    }

    /**
     * Мувающий operator=
     */
    UniString& operator=(UniString&& other) {
        data = std::move(other.data);
        locale = std::move(other.locale);
        return *this;
    }

    /**
     * Обращение по индексу -- менять нельзя
     */
    const UniCharacter& operator[](uint index) const {
        return data->operator[](index);
    }

    uint length() const {
        return data->size();
    }

    bool isEmpty() const {
        return data->empty();
    }

    bool operator==(const UniString& other) const;

    bool operator!=(const UniString& other) const {
        return !(*this == other);
    }

    bool operator==(const std::string& other) const;

    bool operator!=(const std::string& other) const {
        return !(*this == other);
    }

    /**
     * Строка в верхнем регистре
     */
    bool isUpperCase() const;

    /**
     * Строка в нижнем регистре
     */
    bool isLowerCase() const;

    /**
     * Получить строку в верхнем регистре
     */
    UniString toUpperCase() const;

    /**
     * Получить строку в нижнем регистре
     */
    UniString toLowerCase() const;

    /**
     * Найти в строке подстроку
     * начиная со @start
     * В случае неудачи возвращает -1
     */
    long find(const UniString& other, uint start = 0) const;
    /**
     * Найти регулярку
     */
    long find(const boost::u32regex& reg, uint start = 0) const;

    /**
	 * Cодержит ли данный символ
	 */
    long find(const UniCharacter& c, uint start = 0) const;
    /**
     * Разбить по символу
     */
    std::vector<UniString> split(const UniCharacter& chr) const;

    /**
     * Разбить по строке
     */
    std::vector<UniString> split(const UniString& str) const;

    /**
     * Разбить по символу
     */
    std::vector<UniString> split(char chr) const;

    /**
     * Разбить по регулярке
     */
    std::vector<UniString> split(const boost::u32regex& reg) const;

    /**
     * Получить внутреннее представление
     * в виде строки
     */
    std::string getRawString(uint start = 0) const;

    friend std::ostream& operator<<(std::ostream& os, const UniString& str);
    friend std::istream& operator>>(std::istream& is, UniString& str);

    /**
     * Лексикографическое сравнение строк
     */
    bool operator<(const UniString& other) const;

    bool operator>(const UniString& other) const {
        return !(*this < other) && !(*this == other);
    }

    /**
     * Сложить две строки
     */
    UniString operator+(const UniString& other) const;

    /**
     * Сгенерить подстроку
     */
    UniString subString(uint start = 0, uint len = -1) const; // -1 на самом деле npos

    /**
	 * Откусить кусок строки длинной len
	 */
    UniString cut(uint len) const {
        return subString(0, len);
    }

    /**
	 * Откусить кусок строки длинной len с конца
	 */
    UniString rcut(uint len) const {
        if (len >= length())
            return *this;
        return subString(length() - len);
    }
    /**
     * Перевернуть строку
     */
    UniString reverse() const;

    /**
     * Строка оканчивается с
     */
    bool endsWith(const UniString& tail) const;

    /**
     * Строчка начинается с
     */
    bool startsWith(const UniString& head) const;

    /**
     *Строка начнинается с
     */
    inline bool startsWith(const UniCharacter& head) const {
        if (data->empty())
            return false;
        return (*data)[0] == head;
    }
    /**
     * Содержит ли строку подсроку
     */
    bool contains(const UniString& str) const {
        return find(str) != -1;
    }

    /**
     * Содержит ли регулярку
     */
    bool contains(const boost::u32regex& reg) const;

    /**
     * Матчится ли ЦЕЛИКОМ на регулярку
     */
    bool match(const boost::u32regex& reg) const;

    /**
	 * Содержит ли данный символ
	 */
    bool contains(const UniCharacter& c) const {
        return find(c) != -1;
    }

    /**
     * получить локаль
     *
     */
    std::locale getLocale() const {
        return locale;
    }

    /**
	 * Заменить подстроку в строке
	 */
    UniString replace(const UniString& what, const UniString& whereby) const;

    /**
	 * Заменить символ в строке
	 */
    UniString replace(const UniCharacter& what, const UniCharacter& whereby) const;

    /**
	 * Является ли строка числом
	 */
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
        for (int i = 0; i < s.length(); ++i) {
            h1 += std::hash<std::string>{}(s[i].getInnerRepr());
        }
        return h1; // or use boost::hash_combine
    }
};
}

#endif //CROSSTOKENIZER_UNISTRING_H
