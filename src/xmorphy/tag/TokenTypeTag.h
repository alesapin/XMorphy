#pragma once
#include "ITag.h"
namespace X
{
struct TokenTypeTag : public ITag
{
private:
    TokenTypeTag(uint64_t val);
    TokenTypeTag(const std::string & val);
    static const std::vector<TokenTypeTag> inner_runner;

public:
    static const TokenTypeTag UNKN;
    static const TokenTypeTag WORD;
    static const TokenTypeTag PNCT;
    static const TokenTypeTag SEPR;
    static const TokenTypeTag NUMB;
    static const TokenTypeTag WRNM;
    static const TokenTypeTag HIER;

    static std::vector<TokenTypeTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<TokenTypeTag>::const_iterator end() { return inner_runner.end(); }
    bool operator==(const TokenTypeTag & other) const { return value == other.value && name_map == other.name_map; }
    bool operator!=(const TokenTypeTag & other) const { return !this->operator==(other); }
    bool operator<(const TokenTypeTag & other) const { return value < other.value; }
    bool operator>(const TokenTypeTag & other) const { return value > other.value; }
};
}
