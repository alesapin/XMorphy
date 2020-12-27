#pragma once
#include <xmorphy/morph/DictMorphAnalyzer.h>
#include <unordered_set>
namespace X
{
class PrefixAnalyzer : public DictMorphAnalyzer
{
public:
    PrefixAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs, std::istream & prefixDictIs)
        : DictMorphAnalyzer(mainDictIs, affixDictIs), prefDict(loadPrefixDict(prefixDictIs))
    {
    }

    PrefixAnalyzer();

    std::vector<ParsedPtr> analyze(const utils::UniString & str) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & t) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const override;
    bool isDictWord(const utils::UniString & str) const override;

    std::vector<ParsedPtr> generate(const utils::UniString & str) const override;

protected:
    PrefixDict prefDict;
    std::unordered_set<utils::UniString> cutPrefix(const utils::UniString & source) const;
};
}
