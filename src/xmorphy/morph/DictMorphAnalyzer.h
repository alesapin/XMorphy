#pragma once
#include <xmorphy/build/DictBuilder.h>
#include <xmorphy/morph/IMorphAnalyzer.h>
namespace X
{
class DictMorphAnalyzer : public IMorphAnalyzer
{
private:
    UniString buildNormalForm(
        UniString wordForm,
        const UniString & formPrefix,
        const UniString & formSuffix,
        UniString normalFormPrefix,
        const UniString & normalFormSuffix) const;
    ParsedPtr buildByPara(
        const LexemeGroup & reqForm, const LexemeGroup & givenForm, const LexemeGroup & normalForm, const UniString & given, bool only_given_lemma=false) const;

public:
    DictMorphAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs);
    DictMorphAnalyzer();

    std::vector<ParsedPtr> generate(const UniString & str, const std::map<Paradigm, size_t> & paradigms) const;

    virtual std::vector<ParsedPtr> generate(const UniString & str) const;

    std::vector<ParsedPtr> analyze(const UniString & str) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & t) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const override;
    bool isDictWord(const UniString & str) const override;
    bool isWordContainsInDictionary(const UniString & str) const;


protected:
    virtual std::vector<ParsedPtr> analyze(const UniString & str, const std::vector<MorphDictInfo> & dictInfo) const;
    virtual std::vector<ParsedPtr>
    synthesize(const UniString & str, const UniMorphTag & t, const std::map<Paradigm, std::size_t> & paras) const;
    std::vector<ParsedPtr> synthesize(
        const UniString & str,
        const UniMorphTag & given,
        const UniMorphTag & req,
        const std::map<Paradigm, std::size_t> & paras) const;

protected:
    std::unique_ptr<MorphDict> dict;
};
}
