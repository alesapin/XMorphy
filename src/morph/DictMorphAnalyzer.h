#pragma once
#include <build/DictBuilder.h>
#include "IMorphAnalyzer.h"
namespace X
{
class DictMorphAnalyzer : public IMorphAnalyzer
{
private:
    utils::UniString buildNormalForm(
        utils::UniString wordForm,
        const utils::UniString & formPrefix,
        const utils::UniString & formSuffix,
        utils::UniString normalFormPrefix,
        const utils::UniString & normalFormSuffix) const;
    ParsedPtr buildByPara(
        const LexemeGroup & reqForm, const LexemeGroup & givenForm, const LexemeGroup & normalForm, const utils::UniString & given) const;

public:
    DictMorphAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs);
    DictMorphAnalyzer();
    std::vector<ParsedPtr> analyze(const utils::UniString & str) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & t) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const override;
    virtual bool isDictWord(const utils::UniString & str) const override;

protected:
    virtual std::vector<ParsedPtr> analyze(const utils::UniString & str, const std::vector<MorphDictInfo> & dictInfo) const;
    virtual std::vector<ParsedPtr>
    synthesize(const utils::UniString & str, const UniMorphTag & t, const std::map<Paradigm, std::size_t> & paras) const;
    std::vector<ParsedPtr> synthesize(
        const utils::UniString & str,
        const UniMorphTag & given,
        const UniMorphTag & req,
        const std::map<Paradigm, std::size_t> & paras) const;

protected:
    std::unique_ptr<MorphDict> dict;
};
}
