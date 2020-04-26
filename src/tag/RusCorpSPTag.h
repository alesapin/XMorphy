#pragma once
#include "ITag.h"
namespace X
{
struct RusCorpSPTag : public ITag
{
private:
    RusCorpSPTag(uint64_t val);
    static const std::vector<RusCorpSPTag> inner_runner;

public:
    static const RusCorpSPTag UNKN;
    static const RusCorpSPTag S;
    static const RusCorpSPTag A;
    static const RusCorpSPTag NUM;
    static const RusCorpSPTag ANUM;
    static const RusCorpSPTag V;
    static const RusCorpSPTag ADV;
    static const RusCorpSPTag PRAEDIC;
    static const RusCorpSPTag PARENTH;
    static const RusCorpSPTag SPRO;
    static const RusCorpSPTag APRO;
    static const RusCorpSPTag ADVPRO;
    static const RusCorpSPTag PRAEDICPRO;
    static const RusCorpSPTag PR;
    static const RusCorpSPTag CONJ;
    static const RusCorpSPTag PART;
    static const RusCorpSPTag INTJ;
    static const RusCorpSPTag INIT;

    static std::vector<RusCorpSPTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<RusCorpSPTag>::const_iterator end() { return inner_runner.end(); }

    RusCorpSPTag(const std::string & val);
    RusCorpSPTag();
};

}
