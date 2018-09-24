//
// Created by alesapin on 19.06.16.
//

#ifndef CROSSTOKENIZER_TOKEN_H
#define CROSSTOKENIZER_TOKEN_H

#include <utils/UniString.h>
#include <tag/TokenTypeTag.h>
#include <tag/GraphemTag.h>
#include <base/IEntity.h>
#include <type_traits>
#include <functional>


namespace base {

class Token : public IEntity {

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

    Token(const utils::UniString& i, TokenTypeTag t = TokenTypeTag::UNKN, GraphemTag tt = GraphemTag::UNKN) : inner(i), type(t), tag(tt) {}

    virtual TokenTypeTag getType() const {
        return type;
    }
    virtual GraphemTag getTag() const {
        return tag;
    }
    virtual const utils::UniString &getInner() const {
        return inner;
    }
    /**
     * Строковое представление токена
     */
    virtual utils::UniString toString() const {
        return inner;
    }

    virtual uint getStartPos() const {
        return startPos;
    }
    /**
     * Длинна токена
     */
    virtual uint getLength() const {
        return inner.length();
    }

    friend std::ostream& operator<<(std::ostream& os, const Token& tok){
        return os << tok.toString();
    }
};

typedef std::shared_ptr<Token> TokenPtr;
}


#endif //CROSSTOKENIZER_TOKEN_H
