#pragma once

#include <functional>
#include <type_traits>
#include <xmorphy/tag/GraphemTag.h>
#include <xmorphy/tag/TokenTypeTag.h>
#include <xmorphy/utils/UniString.h>


namespace X
{
class Token
{
    typedef std::size_t uint;

protected:
    /**
     * Текст внутри токена
     */
    UniString inner;
    /**
     * Тип токена
     */
    const TokenTypeTag type;
    /**
     * Графематическая информация
     */
    const GraphemTag tag;
    /**
     * Позиция токена в тексте
     */
    size_t start_pos_unicode;

    /**
     * Позиция токена в массиве байт
     */
    size_t start_pos_byte;

public:
    Token(const UniString & i, TokenTypeTag t = TokenTypeTag::UNKN, GraphemTag tt = GraphemTag::UNKN) : inner(i), type(t), tag(tt) {}

    TokenTypeTag getType() const { return type; }
    GraphemTag getTag() const { return tag; }
    const UniString & getInner() const { return inner; }
    /**
     * Строковое представление токена
     */
    UniString toString() const { return inner; }

    void setStartPosUnicode(size_t pos) { start_pos_unicode = pos; }
    size_t getStartPosUnicode() const { return start_pos_unicode; }

    void setStartPosByte(size_t pos) { start_pos_byte = pos; }
    size_t getStartPosByte() const { return start_pos_byte; }

    /**
     * Длинна токена
     */
    size_t getLength() const { return inner.length(); }

    friend std::ostream & operator<<(std::ostream & os, const Token & tok) { return os << tok.toString(); }

    bool operator==(const Token & other) const
    {
        return inner == other.inner && type == other.type && tag == other.tag;
    }
};

using TokenPtr = std::shared_ptr<Token>;
}
