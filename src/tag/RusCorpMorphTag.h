#pragma once
#include "ITag.h"
namespace X
{
struct RusCorpMorphTag : public ITag
{
private:
    RusCorpMorphTag(uint64_t val);
    static const std::vector<RusCorpMorphTag> inner_runner;

public:
    static const RusCorpMorphTag UNKN;
    // Gender
    static const RusCorpMorphTag m;
    static const RusCorpMorphTag f;
    static const RusCorpMorphTag m_f;
    static const RusCorpMorphTag n;
    // Animacy
    static const RusCorpMorphTag anim;
    static const RusCorpMorphTag inan;
    // Number
    static const RusCorpMorphTag sg;
    static const RusCorpMorphTag pl;
    // Case
    static const RusCorpMorphTag nom;
    static const RusCorpMorphTag gen;
    static const RusCorpMorphTag dat;
    static const RusCorpMorphTag acc;
    static const RusCorpMorphTag ins;
    static const RusCorpMorphTag loc;
    static const RusCorpMorphTag gen2;
    static const RusCorpMorphTag acc2;
    static const RusCorpMorphTag loc2;
    static const RusCorpMorphTag voc;
    static const RusCorpMorphTag adnum;
    // Degree of comparison
    static const RusCorpMorphTag comp;
    static const RusCorpMorphTag comp2;
    static const RusCorpMorphTag supr;
    // VerbForm
    static const RusCorpMorphTag inf;
    static const RusCorpMorphTag partcp;
    static const RusCorpMorphTag ger;
    // Mood
    static const RusCorpMorphTag indic;
    static const RusCorpMorphTag imper;
    static const RusCorpMorphTag imper2;
    // Person
    static const RusCorpMorphTag _1p;
    static const RusCorpMorphTag _2p;
    static const RusCorpMorphTag _3p;
    // Tense
    static const RusCorpMorphTag praet;
    static const RusCorpMorphTag praes;
    static const RusCorpMorphTag fut;
    // Shortness
    static const RusCorpMorphTag brev;
    static const RusCorpMorphTag plen;
    // Voice
    static const RusCorpMorphTag act;
    static const RusCorpMorphTag pass;
    static const RusCorpMorphTag med;
    // Perehod
    static const RusCorpMorphTag intr;
    static const RusCorpMorphTag tran;
    // Other
    static const RusCorpMorphTag persn;
    static const RusCorpMorphTag patrn;
    static const RusCorpMorphTag famn;
    static const RusCorpMorphTag zoon;
    static const RusCorpMorphTag _0;
    static const RusCorpMorphTag abbr;
    static const RusCorpMorphTag distort;
    static const RusCorpMorphTag ciph;
    static const RusCorpMorphTag anom;
    //Vid
    static const RusCorpMorphTag pf;
    static const RusCorpMorphTag ipf;


    static std::vector<RusCorpMorphTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<RusCorpMorphTag>::const_iterator end() { return inner_runner.end(); }

    RusCorpMorphTag(const std::string & val);
    RusCorpMorphTag();
};
}
