//
// Created by alesapin on 16.06.16.
//

#include "UniCharacter.h"
namespace utils {
bool UniCharacter::isalpha() const {
    return tag & CharTag::LETTER;
}

bool UniCharacter::ispunct() const {
    return tag & CharTag::PUNCT;
}

bool UniCharacter::isdigit() const {
    return tag & CharTag::DIGIT;
}

bool UniCharacter::isupper() const {
    return tag & CharTag::UPPERCASE;
}

bool UniCharacter::islower() const {
    return tag & CharTag::LOWERCASE;
}

bool UniCharacter::issep() const {
    return tag & CharTag::SEPARATOR;
}

bool UniCharacter::iscntrl() const {
    return tag & CharTag::CNTRL;
}

std::string UniCharacter::getInnerRepr() const {
    return peace;
}

std::ostream& operator<<(std::ostream& os, const UniCharacter& chr) {
    return os << chr.peace;
}

CharTag detectTag(const UniCharacter& chr, const std::locale& loc) {
    std::string repr = chr.getInnerRepr();
    return detectTag(repr, loc);
}

CharTag detectTag(char chr) {
    if (std::isalpha(chr)) {
        if (std::isupper(chr)) {
            return static_cast<CharTag>(CharTag::LETTER | CharTag::UPPERCASE);
        } else {
            return static_cast<CharTag>(CharTag::LETTER | CharTag::LOWERCASE);
        }
    } else if (std::isdigit(chr)) {
        return CharTag::DIGIT;
    } else if (std::isspace(chr)) {
        return CharTag::SEPARATOR;
    } else if (std::ispunct(chr)) {
        return CharTag::PUNCT;
    } else if (std::iscntrl(chr)) {
        return CharTag::CNTRL;
    }
    return CharTag::UNKNOWN;
}

CharTag detectTag(const std::string& repr, const std::locale& loc) {
    using namespace boost::locale::boundary;
    static std::unordered_map<std::string, CharTag> char_cache;
    if (char_cache.count(repr))
        return char_cache[repr];

    if (repr.length() == 1) {
        char_cache[repr] = detectTag(repr[0]);
    } else {
        CharTag t = UNKNOWN;
        ssegment_index tmp(word, repr.begin(), repr.end(), loc);
        ssegment_index::iterator b = tmp.begin();
        if (b->rule() & word_number) {
            char_cache[repr] = CharTag::DIGIT;
        } else if (b->rule() & word_letter) {
            t = static_cast<CharTag>(t | CharTag::LETTER);
            if (boost::locale::to_upper(repr, loc) == repr) {
                t = static_cast<CharTag>(t | CharTag::UPPERCASE);
            } else {
                t = static_cast<CharTag>(t | CharTag::LOWERCASE);
            }
            char_cache[repr] = t;
        } else if (b->rule() & word_kana_ideo) {
            char_cache[repr] = CharTag::HIEROGLYPH;
        } else if (b->rule() & word_none) {
            char_cache[repr] = CharTag::PUNCT;
        } else {
            char_cache[repr] = CharTag::UNKNOWN;
        }
    }
    return char_cache[repr];
}

bool UniCharacter::isOneByte() const {
    return peace.length() == 1;
}

UniCharacter& UniCharacter::operator=(const UniCharacter& o) {
    peace = o.peace;
    tag = o.tag;
    return *this;
}

UniCharacter::UniCharacter(const UniCharacter& o)
    : peace(o.peace)
    , tag(o.tag) {
}

UniCharacter UniCharacter::toUpper(const std::locale& loc) const {
    if (!isalpha())
        return *this;
    std::string upStr = boost::locale::to_upper(peace, loc);
    return UniCharacter(upStr, static_cast<CharTag>((tag & ~CharTag::LOWERCASE) | CharTag::UPPERCASE));
}

UniCharacter UniCharacter::toLower(const std::locale& loc) const {
    if (!isalpha())
        return *this;
    std::string lowStr = boost::locale::to_lower(peace, loc);
    return UniCharacter(lowStr, static_cast<CharTag>((tag & ~CharTag::UPPERCASE) | CharTag::LOWERCASE));
}

bool UniCharacter::operator<(const UniCharacter& other) const {
    if (peace.size() < other.peace.size())
        return true;
    else if (peace.size() > other.peace.size())
        return false;
    uint val1 = 0, val2 = 0;
    for (size_t i = 0; i < peace.size(); ++i) {
        val1 += peace[i];
        val2 += other.peace[i];
    }
    return val1 < val2;
}

UniCharacter::UniCharacter(const std::string& sym, const std::locale& loc) {
    peace = sym;
    tag = detectTag(*this, loc);
}

bool UniCharacter::ishieroglyph() const {
    return tag & CharTag::HIEROGLYPH;
}

UniCharacter::UniCharacter(UniCharacter&& o)
    : peace(std::move(o.peace))
    , tag(std::move(o.tag)) {
}

UniCharacter& UniCharacter::operator=(UniCharacter&& other) {
    peace = std::move(other.peace);
    tag = std::move(other.tag);
    return *this;
}

const UniCharacter UniCharacter::A("А", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::B("Б", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::V("В", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::G("Г", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::D("Д", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::YE("Е", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::YO("Ё", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::JE("Ж", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::ZE("З", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::I("И", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::YI("Й", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::K("К", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::L("Л", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::M("М", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::N("Н", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::O("О", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::P("П", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::R("Р", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::S("С", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::T("Т", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::U("У", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::F("Ф", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::H("Х", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::CE("Ц", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::CH("Ч", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::SH("Ш", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::SHE("Щ", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::TV("Ъ", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::AUY("Ы", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::MG("Ь", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::AE("Э", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::UY("Ю", (CharTag)(LETTER | UPPERCASE));
const UniCharacter UniCharacter::YA("Я", (CharTag)(LETTER | UPPERCASE));

const std::set<UniCharacter> UniCharacter::VOWELS = {UniCharacter::A, UniCharacter::YO, UniCharacter::O,
                                                     UniCharacter::YE, UniCharacter::I, UniCharacter::U,
                                                     UniCharacter::AUY, UniCharacter::AE, UniCharacter::UY,
                                                     UniCharacter::YA};
}
