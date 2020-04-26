#pragma once

#include <functional>
#include <type_traits>
#include <tag/GraphemTag.h>
#include <tag/TokenTypeTag.h>
#include <utils/UniString.h>


namespace X
{
class Token
{
    typedef std::size_t uint;

protected:
    /**
     * Текст внутри токена
     */
    utils::UniString inner;
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
    uint startPos;

public:
    Token(const utils::UniString & i, TokenTypeTag t = TokenTypeTag::UNKN, GraphemTag tt = GraphemTag::UNKN) : inner(i), type(t), tag(tt) {}

    TokenTypeTag getType() const { return type; }
    GraphemTag getTag() const { return tag; }
    const utils::UniString & getInner() const { return inner; }
    /**
     * Строковое представление токена
     */
    utils::UniString toString() const { return inner; }

    uint getStartPos() const { return startPos; }
    /**
     * Длинна токена
     */
    uint getLength() const { return inner.length(); }

    friend std::ostream & operator<<(std::ostream & os, const Token & tok) { return os << tok.toString(); }
};

using TokenPtr = std::shared_ptr<Token>;
}
