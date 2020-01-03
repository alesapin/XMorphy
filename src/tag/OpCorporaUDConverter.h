#ifndef OP_CORPORA_UD_CONVERTER_H
#define OP_CORPORA_UD_CONVERTER_H
#include <tag/ITagConverter.h>
#include <tag/MorphTag.h>
#include <tag/SpeechPartTag.h>
#include <tag/UniSPTag.h>
#include <tag/UniMorphTag.h>
#include <morph/WordForm.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace base {

namespace pt = boost::property_tree;
class OpCorporaUDConverter {
    std::set<utils::UniString> adps;
    std::set<utils::UniString> conjs;
    std::set<utils::UniString> dets;
    std::set<utils::UniString> Hs;
    std::set<utils::UniString> parts;
    std::set<utils::UniString> prons;
    std::set<utils::UniString> fakeAdjs;

    void adjRule(analyze::MorphInfo& mi, const SpeechPartTag& sp, MorphTag& mt) const;
    void adjRule(analyze::WordFormPtr wf) const;

    void verbRule(analyze::MorphInfo& mi, const SpeechPartTag& sp, MorphTag& mt, bool tsya) const;

    void compRule(analyze::WordFormPtr wf) const;

    void restRuleSP(analyze::MorphInfo& mi, const SpeechPartTag& sp, MorphTag &mt, const utils::UniString &wf) const;

    void restRuleMT(analyze::MorphInfo& mi, MorphTag& mt) const;
    void staticRule(const utils::UniString& wordform, const utils::UniString& notupper, std::vector<analyze::MorphInfo>& infos) const;

    void parseTag(const std::string& path, std::set<utils::UniString>& set, pt::ptree &ptree);

public:
    OpCorporaUDConverter(const std::string& confpath);
    void convert(analyze::WordFormPtr wf) const;
};
}
#endif
