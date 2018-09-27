#include "Tokenizer.h"
namespace tokenize {
std::vector<std::shared_ptr<base::Token>> Tokenizer::analyze(const utils::UniString& text) const {
    std::vector<std::shared_ptr<base::Token>> result;
    for (uint i = 0; i < text.length();) {
        std::shared_ptr<base::Token> r;
        uint nextI = i;
        if (text[i].isalpha()) {
            nextI = cutWord(i, text);
            bool notAword = false;
            if (nextI == i) { //Не продвинулись --> не слово
                nextI = cutWordNum(i, text);
                notAword = true;
            }
            utils::UniString word = text.subString(i, nextI - i);
            if (notAword) {
                r = processWordNum(word);
            } else {
                r = processWord(word);
            }
        } else if (text[i].ispunct()) {
            nextI = cutPunct(i, text);
            utils::UniString punct = text.subString(i, nextI - i);
            r = processPunct(punct);
        } else if (text[i].isdigit()) {
            nextI = cutNumber(i, text);
            bool notAnumber = false;
            if (nextI == i) { //Не продвинулись --> не цифра
                nextI = cutWordNum(i, text);
                notAnumber = true;
            }
            utils::UniString num = text.subString(i, nextI - i);
            if (notAnumber) {
                r = processWordNum(num);
            } else {
                r = processNumber(num);
            }
        } else if (text[i].issep()) {
            nextI = cutSeparator(i, text);
            utils::UniString sep = text.subString(i, nextI - i);
            r = processSeparator(sep);
        } else if (text[i].iscntrl() || text[i].ishieroglyph()) {
            nextI = cutTrash(i, text);
            utils::UniString trash = text.subString(i, nextI - i);
            r = processHieroglyph(trash);
        }
        i = nextI;
        result.push_back(r);
    }
    return result;
}

std::shared_ptr<base::Token> Tokenizer::analyzeSingleWord(const utils::UniString& word) const {
        return processWord(word);
}

uint Tokenizer::cutWordNum(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() && (str[i].isalpha() || str[i].isdigit())) {
        ++i;
    }
    return i;
}

uint Tokenizer::cutNumber(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() &&
           str[i].isdigit()) {
        ++i;
    }
    //Если закончили не на пунктуации или
    //разделителе, значит это не цифра
    if (i < str.length() && !str[i].issep() && !str[i].ispunct()) {
        return start;
    }
    return i;
}

uint Tokenizer::cutSeparator(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() &&
           str[i].issep()) {
        i++;
    }
    return i;
}

uint Tokenizer::cutPunct(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() &&
           str[i].ispunct() &&
           str[i] == utils::UniCharacter(".")) {
        ++i;
    }
    if (i == start)
        return i + 1;
    return i;
}

uint Tokenizer::cutWord(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() &&
           str[i].isalpha()) {
        i++;
    }
    if (i < str.length() && !str[i].issep() && !str[i].ispunct()) {
        return start;
    }
    return i;
}

uint Tokenizer::cutTrash(uint start, const utils::UniString& str) const {
    uint i = start;
    while (i < str.length() &&
           (str[i].ishieroglyph() || str[i].iscntrl())) {
        i++;
    }

    return i;
}
//    UNKNOWN = 0x00,
//    CYRILLIC = 0x01,
//    LATIN = 0x02,
//    UPPER_CASE = 0x04,
//    LOWER_CASE = 0x08,
//    MIXED_CASE = 0x10,
//    CAP_START = 0x30, // Начинается с большой буквы (полюбому mixed_case)
//    ABBR = 0x40, //Аббревиатура
//    NAM_ENT = 0x80, // именованная сущность (Город, Имя, ...)
//    MULTI_WORD = 0x100, //Несколько слов, актуально для именованных сущностей
//    SINGLE_WORD = 0x200, //Одно слово
//    WORD_TAG_END = 0x400,

//std::shared_ptr<base::Token> Tokenizer::processWord(const utils::UniString &str) const {
//    base::Word *res = new base::Word(str);
//    base::WordTag t = base::WordTag
//    for(uint i = 0; i < str.length(); ++i) {
//    }
//
//

//    return std::unique_ptr<base::Word>(res);
//}
std::shared_ptr<base::Token> Tokenizer::processWord(const utils::UniString& str) const {
    base::GraphemTag t = base::GraphemTag::UNKN;
    bool isUpperCase = true;
    bool isLowerCase = true;
    bool isLatin = true;
    bool isCyrrilic = true;
    bool capStart = str[0].isupper();
    int capCounter = 0;
    for (const utils::UniCharacter& chr : str) {
        if (!chr.isOneByte()) {
            isLatin = false;
        } else {
            isCyrrilic = false;
        }
        if (chr.islower()) {
            isUpperCase = false;
        } else {
            capCounter++;
            isLowerCase = false;
        }
    }
    if (isUpperCase) {
        t |= base::GraphemTag::UPPER_CASE;
    } else if (isLowerCase) {
        t |= base::GraphemTag::LOWER_CASE;
    } else if (capCounter == 1 && capStart) {
        t |= base::GraphemTag::CAP_START;
    } else {
        t |= base::GraphemTag::MIXED;
    }
    if (isLatin) {
        t |= base::GraphemTag::LATIN;
    } else if (isCyrrilic) {
        t |= base::GraphemTag::CYRILLIC;
    } else {
        t |= base::GraphemTag::MULTI_ENC;
    }
    base::Token* res = new base::Token(str, base::TokenTypeTag::WORD, t);
    return std::shared_ptr<base::Token>(res);
}

std::shared_ptr<base::Token> Tokenizer::processPunct(const utils::UniString& str) const {
    base::GraphemTag t = base::GraphemTag::UNKN;
    if (str.length() > 1) {
        t |= base::GraphemTag::PUNCT_GROUP;
        bool isThreeDots = true;
        for (const utils::UniCharacter& t : str) {
            if (t != ".") {
                isThreeDots = false;
            }
        }
        if (isThreeDots) {
            t |= base::GraphemTag::THREE_DOTS;
        }
    } else {
        utils::UniCharacter sym = str[0];
        if (sym == ",") {
            t |= base::GraphemTag::COMMA;
        } else if (sym == ".") {
            t |= base::GraphemTag::DOT;
        } else if (sym == ":") {
            t |= base::GraphemTag::COLON;
        } else if (sym == ";") {
            t |= base::GraphemTag::SEMICOLON;
        } else if (sym == "?") {
            t |= base::GraphemTag::QUESTION_MARK;
        } else if (sym == "!") {
            t |= base::GraphemTag::EXCLAMATION_MARK;
        } else if (sym == "\"" || sym == "»" || sym == "«") {
            t |= base::GraphemTag::QUOTE;
        } else if (sym == "_") {
            t |= base::GraphemTag::LOWER_DASH;
        } else if (sym == "-" || sym == "—") {
            t |= base::GraphemTag::DASH;
        } else if (sym == "(") {
            t |= base::GraphemTag::PARENTHESIS_L;
        } else if (sym == ")") {
            t |= base::GraphemTag::PARENTHESIS_R;
        } else {
            t |= base::GraphemTag::UNCOMMON_PUNCT;
        }
    }
    base::Token* res = new base::Token(str, base::TokenTypeTag::PNCT, t);
    return std::shared_ptr<base::Token>(res);
}
std::shared_ptr<base::Token> Tokenizer::processNumber(const utils::UniString& number) const {
    bool isBinary = true;
    bool isOct = number[0] == "0";
    base::GraphemTag t = base::GraphemTag::DECIMAL;
    for (const utils::UniCharacter& chr : number) {
        if (chr != "0" && chr != "1") {
            isBinary = false;
        }
        if (chr == "8" || chr == "9") {
            isOct = false;
        }
    }
    if (isBinary) {
        t |= base::GraphemTag::BINARY;
    } else if (isOct) {
        t |= base::GraphemTag::OCT;
    }
    base::Token* res = new base::Token(number, base::TokenTypeTag::NUMB, t);
    return std::shared_ptr<base::Token>(res);
}
std::shared_ptr<base::Token> Tokenizer::processSeparator(const utils::UniString& sep) const {
    base::GraphemTag t = base::GraphemTag::UNKN;
    if (sep.length() > 1) {
        t = base::GraphemTag::MULTI_SEP;
    } else {
        t = base::GraphemTag::SINGLE_SEP;
        utils::UniCharacter sym = sep[0];
        if (sym == " ") {
            t |= base::GraphemTag::SPACE;
        } else if (sym == "\t") {
            t |= base::GraphemTag::TAB;
        } else if (sym == "\n") {
            t |= base::GraphemTag::NEW_LINE;
        } else if (sym == "\r") {
            t |= base::GraphemTag::CR;
        }
    }
    base::Token* res = new base::Token(sep, base::TokenTypeTag::SEPR, t);
    return std::shared_ptr<base::Token>(res);
}
std::shared_ptr<base::Token> Tokenizer::processWordNum(const utils::UniString& wn) const {
    base::GraphemTag t = base::GraphemTag::UNKN;
    bool stop = false;
    for (std::size_t i = wn.length() - 1; i > 0; --i) {
        if (wn[i].isOneByte() && !stop) {
            stop = true;
        } else if (!wn[i].isOneByte() && stop) {
            stop = false;
            break;
        }
    }
    if (stop) {
        t |= base::GraphemTag::CYRILLIC;
    }
    base::Token* res = new base::Token(wn, base::TokenTypeTag::WRNM, t);
    return std::shared_ptr<base::Token>(res);
}

std::shared_ptr<base::Token> Tokenizer::processHieroglyph(const utils::UniString& hir) const {
    base::Token* res = new base::Token(hir, base::TokenTypeTag::HIER);
    return std::shared_ptr<base::Token>(res);
}
}
