#ifndef _DICT_MORPH_ANALYZER
#define _DICT_MORPH_ANALYZER
#include "IMorphAnalyzer.h"
#include <build/DictBuilder.h>
namespace analyze {
class DictMorphAnalyzer : public IMorphAnalyzer {
private:
    utils::UniString buildNormalForm(utils::UniString wordForm,
                                     utils::UniString formPrefix,
                                     utils::UniString formSuffix,
                                     utils::UniString normalFormPrefix,
                                     utils::UniString normalFormSuffix) const;
    ParsedPtr buildByPara(const build::LexemeGroup& reqForm, const build::LexemeGroup& givenForm, const build::LexemeGroup& normalForm, const utils::UniString& given) const;

public:
    DictMorphAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs);
    std::vector<ParsedPtr> analyze(const utils::UniString& str) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString& str, const base::MorphTag& t) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString& str, const base::MorphTag& given, const base::MorphTag& req) const override;
    virtual bool isDictWord(const utils::UniString& str) const override;

protected:
    virtual std::vector<ParsedPtr> analyze(const utils::UniString& str, const std::vector<std::tuple<build::LexemeGroup, build::AffixPair, std::size_t>>& dictInfo) const;
    virtual std::vector<ParsedPtr> synthesize(const utils::UniString& str, const base::MorphTag& t, const std::map<build::Paradigm, std::size_t>& paras) const;
    std::vector<ParsedPtr> synthesize(const utils::UniString& str, const base::MorphTag& given, const base::MorphTag& req, const std::map<build::Paradigm, std::size_t>& paras) const;
    std::unique_ptr<build::MorphDict> dict;
};
}
#endif
