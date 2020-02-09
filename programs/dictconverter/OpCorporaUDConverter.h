#pragma once
#include <tag/MorphTag.h>
#include <tag/SpeechPartTag.h>
#include <tag/UniSPTag.h>
#include <tag/UniMorphTag.h>
#include <morph/WordForm.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

struct ConvertMorphInfo
{
    utils::UniString normalForm;
    base::MorphTag tag;
    base::SpeechPartTag sp;
    base::UniMorphTag utag;
    base::UniSPTag usp;

    bool operator<(const ConvertMorphInfo & other) const
    {
        return std::tie(normalForm, tag, sp, utag, usp) < std::tie(other.normalForm, other.tag, other.sp, other.utag, other.usp);
    }
};

std::ostream & operator<<(std::ostream & os, const ConvertMorphInfo & info);

struct ConvertWordForm
{
    utils::UniString wordForm;
    std::set<ConvertMorphInfo> infos;
    base::TokenTypeTag tokenTag = base::TokenTypeTag::UNKN;
    base::GraphemTag graphemTag = base::GraphemTag::UNKN;
};

class OpCorporaUDConverter {
    std::set<utils::UniString> adps;
    std::set<utils::UniString> conjs;
    std::set<utils::UniString> dets;
    std::set<utils::UniString> Hs;
    std::set<utils::UniString> parts;
    std::set<utils::UniString> prons;
    std::set<utils::UniString> fakeAdjs;

    void adjRule(ConvertMorphInfo& mi, const base::SpeechPartTag& sp, base::MorphTag& mt) const;
    void adjRule(ConvertWordForm & wf) const;

    void verbRule(ConvertMorphInfo & mi, const base::SpeechPartTag& sp, base::MorphTag& mt, bool tsya) const;

    void compRule(ConvertWordForm & wf) const;

    void restRuleSP(ConvertMorphInfo& mi, const base::SpeechPartTag& sp, base::MorphTag &mt, const utils::UniString & wf) const;

    void restRuleMT(ConvertMorphInfo & mi, base::MorphTag & mt) const;
    void staticRule(const utils::UniString & wordform, const utils::UniString & notupper, std::vector<ConvertMorphInfo> & infos) const;

    void parseTag(const std::string & path, std::set<utils::UniString> & set, pt::ptree & ptree);

public:
    OpCorporaUDConverter(const std::string & confpath);
    void convert(ConvertWordForm & wf) const;
};
