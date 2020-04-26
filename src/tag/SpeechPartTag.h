#pragma once
#include "ITag.h"
namespace X
{
struct SpeechPartTag : public ITag
{
private:
    SpeechPartTag(uint64_t val);
    static const std::vector<SpeechPartTag> inner_runner;

public:
    static const SpeechPartTag UNKN;
    static const SpeechPartTag NOUN;
    static const SpeechPartTag ADJF;
    static const SpeechPartTag ADJS;
    static const SpeechPartTag COMP;
    static const SpeechPartTag VERB;
    static const SpeechPartTag INFN;
    static const SpeechPartTag PRTF;
    static const SpeechPartTag PRTS;
    static const SpeechPartTag GRND;
    static const SpeechPartTag NUMR;
    static const SpeechPartTag ADVB;
    static const SpeechPartTag NPRO;
    static const SpeechPartTag PRED;
    static const SpeechPartTag PREP;
    static const SpeechPartTag CONJ;
    static const SpeechPartTag PRCL;
    static const SpeechPartTag INTJ;

    static std::vector<SpeechPartTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<SpeechPartTag>::const_iterator end() { return inner_runner.end(); }

    static constexpr std::size_t size() { return 18; }

    static SpeechPartTag get(std::size_t index) { return inner_runner[index]; }
    static std::size_t get(SpeechPartTag t) { return std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin(); }

    SpeechPartTag(const std::string & val);
    SpeechPartTag();

    virtual ~SpeechPartTag() {}
};

const std::set<SpeechPartTag> NON_DERIVATIVE_SP{{
    SpeechPartTag::ADVB,
    SpeechPartTag::CONJ,
    SpeechPartTag::INTJ,
    SpeechPartTag::PRED,
    SpeechPartTag::PREP,
    SpeechPartTag::PRCL,
    SpeechPartTag::NUMR,
}};
}
