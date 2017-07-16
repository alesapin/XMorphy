//
// Created by alesapin on 16.06.16.
//

#ifndef CROSSTOKENIZER_UNICHARACTER_H
#define CROSSTOKENIZER_UNICHARACTER_H

#include <string>
#include <iostream>
#include <boost/locale.hpp>

namespace utils {
/**
 * Теги, которые могут быть у символа
 */
enum CharTag {
    UNKNOWN = 0x00,
    LETTER = 0x01,
    PUNCT = 0x02,
    SEPARATOR = 0x04,
    DIGIT = 0x08,
    CNTRL = 0x10,
    UPPERCASE = 0x20,
    LOWERCASE = 0x40,
    HIEROGLYPH = 0x80, //Иероглиф
};
/**
 * Класс представляющий
 * юникодный символ
 */
class UniCharacter {
private:
    std::string peace;
    CharTag tag;
public:

    UniCharacter():peace(""),tag(CharTag::UNKNOWN){}

    UniCharacter(const std::string& p, CharTag t):peace(p),tag(t){}

    UniCharacter(const std::string& sym, const std::locale& loc = std::locale());

    UniCharacter& operator=(const UniCharacter& o);

    UniCharacter& operator=(UniCharacter&& other);


    UniCharacter(const UniCharacter& o);

    UniCharacter(UniCharacter&& o);


    CharTag getTag() const {
        return tag;
    }

    void setTag(CharTag tag) {
        UniCharacter::tag = tag;
    }

    /**
     * Является ли буквой
     */
    bool isalpha() const;
    /**
     * Является ли
     * не буквой,
     * не цифрой,
     * не разделителем
     */
    bool ispunct() const;
    /**
     * Является ли цифрой
     */
    bool isdigit() const;
    /**
     * Буква верхнего регистра
     */
    bool isupper() const;
    /**
     * Буква нижнего регистра
     */
    bool islower() const;
    /**
     * Является ли разделителем
     */
    bool issep() const;
    /**
     * Является ли контрольным символом
     */
    bool iscntrl() const;
    /**
     * Иероглиф
     */
    bool ishieroglyph() const;

    /**
     * Однобайтный ли символ
     */
    bool isOneByte() const;
    /**
     * Размер символа в байтах [1--4]
     */
    uint8_t getByteSize() const {
        return static_cast<uint8_t>(peace.length());
    }
    /**
     * Возвращает внутреннее представление
     */
    std::string getInnerRepr() const;
    /**
     * Для печати
     */
    friend std::ostream& operator<<(std::ostream& os,const UniCharacter& chr);



    /**
     * Сравнить с себе подобным
     */
    bool operator==(const UniCharacter& other) const {
        return peace == other.peace;
    }
    bool operator!=(const UniCharacter& other) const {
        return !(*this == other);
    }
    /**
     * Равны если равны внутренние представления
     */
    bool operator==(const std::string& other) const {
        return peace == other;
    }
    bool operator!=(const std::string& other) const {
        return !(*this == other);
    }
    /**
     * Равны если это односимвольная строка
     * равная c
     */
    bool operator==(char c) const {
        return peace.length() == 1 ? peace[0] == c : false;
    }
    bool operator!=(char c) const {
        return !(*this == c);
    }
    /**
     * Сравнение двух символов
     */
    bool operator<(const UniCharacter& other) const;
    bool operator>(const UniCharacter& other) const{
        return !(*this < other) && !(*this == other);
    }
    /**
     * Символ к верхнему регистру
     * если не буква, то без изменений
     */
    UniCharacter toUpper(const std::locale& loc = std::locale()) const;
    /**
     * Символ к нижнему регистру
     * если не буква, то без именний
     */
    UniCharacter toLower(const std::locale& loc = std::locale()) const;

    /**
     * Буквы русского языка....
     */
    static const UniCharacter A;
    static const UniCharacter B;
    static const UniCharacter V;
    static const UniCharacter G;
    static const UniCharacter D;
	static const UniCharacter YO;
	static const UniCharacter YE;
    static const UniCharacter JE;
    static const UniCharacter ZE;
    static const UniCharacter I;
    static const UniCharacter YI;
    static const UniCharacter K;
    static const UniCharacter L;
    static const UniCharacter M;
    static const UniCharacter N;
    static const UniCharacter O;
    static const UniCharacter P;
    static const UniCharacter R;
    static const UniCharacter S;
    static const UniCharacter T;
    static const UniCharacter U;
    static const UniCharacter F;
    static const UniCharacter H;
    static const UniCharacter CE;
    static const UniCharacter CH;
    static const UniCharacter SH;
    static const UniCharacter SHE;
    static const UniCharacter TV;
    static const UniCharacter AUY;
    static const UniCharacter MG;
    static const UniCharacter AE;
    static const UniCharacter UY;
    static const UniCharacter YA;

    static const std::set<UniCharacter> VOWELS;
};
/**
 * Определить тег символа
 */
CharTag detectTag(const UniCharacter& chr, const std::locale& loc = std::locale());
CharTag detectTag(const std::string& chr, const std::locale& loc = std::locale());
CharTag detectTag(char chr);
}

#endif //CROSSTOKENIZER_UNICHARACTER_H
