#pragma once
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/tag/MorphTag.h>
#include <xmorphy/tag/SpeechPartTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>

namespace pt = boost::property_tree;
using namespace X;

struct ConvertMorphInfo
{
    UniString normalForm;
    MorphTag tag;
    SpeechPartTag sp;
    UniMorphTag utag;
    UniSPTag usp;

    bool operator<(const ConvertMorphInfo & other) const
    {
        return std::tie(normalForm, tag, sp, utag, usp) < std::tie(other.normalForm, other.tag, other.sp, other.utag, other.usp);
    }
};

std::ostream & operator<<(std::ostream & os, const ConvertMorphInfo & info);

struct ConvertWordForm
{
    UniString wordForm;
    std::set<ConvertMorphInfo> infos;
    TokenTypeTag tokenTag = TokenTypeTag::UNKN;
    GraphemTag graphemTag = GraphemTag::UNKN;
    bool isNormalForm;
};

class OpCorporaUDConverter
{
    std::set<UniString> adps;
    std::set<UniString> conjs;
    std::set<UniString> dets;
    std::set<UniString> Hs;
    std::set<UniString> parts;
    std::set<UniString> prons;
    std::set<UniString> fakeAdjs;

    void nounRule(ConvertWordForm & wf) const;

    void adjRule(ConvertMorphInfo & mi, const SpeechPartTag & sp, MorphTag & mt) const;
    void adjRule(ConvertWordForm & wf) const;

    void verbRule(ConvertWordForm & wf) const;

    void verbRule(ConvertMorphInfo & mi, const SpeechPartTag & sp, MorphTag & mt, bool tsya) const;

    void compRule(ConvertWordForm & wf) const;

    void restRuleSP(ConvertMorphInfo & mi, const SpeechPartTag & sp, MorphTag & mt, const UniString & wf) const;

    void restRuleMT(ConvertMorphInfo & mi, MorphTag & mt) const;
    void staticRule(const UniString & wordform, const UniString & notupper, std::vector<ConvertMorphInfo> & infos) const;

    void parseTag(const std::string & path, std::set<UniString> & set, pt::ptree & ptree);

public:
    OpCorporaUDConverter(const std::string & confpath);
    void convert(ConvertWordForm & wf) const;
};
