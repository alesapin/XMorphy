#ifndef TOKEN_TYPE_TAG_H
#define TOKEN_TYPE_TAG_H
#include "ITag.h"
namespace base {

struct TokenTypeTag : public ITag {
private:
    TokenTypeTag(uint128_t val);
    TokenTypeTag(const std::string &val);
    static const std::vector<TokenTypeTag> inner_runner;
public:
    static const TokenTypeTag UNKN;
    static const TokenTypeTag WORD;
    static const TokenTypeTag PNCT;
    static const TokenTypeTag SEPR;
    static const TokenTypeTag NUMB;
    static const TokenTypeTag WRNM;
    static const TokenTypeTag HIER;

    static std::vector<TokenTypeTag>::const_iterator begin() {
        return inner_runner.begin();
    }

    static std::vector<TokenTypeTag>::const_iterator end() {
        return inner_runner.end();
    }
};
}
#endif