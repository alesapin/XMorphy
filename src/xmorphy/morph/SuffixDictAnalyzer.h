#pragma once
#include <xmorphy/build/SuffixDict.h>
#include <xmorphy/morph/PrefixAnalyzer.h>
namespace X
{
class SuffixDictAnalyzer : public PrefixAnalyzer
{
protected:
    std::unique_ptr<SuffixDict> sufDict;
    std::unordered_map<size_t, size_t> suffixesLength;

public:
    SuffixDictAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs, std::istream & prefixDictIs, std::istream & suffixDictIs)
        : PrefixAnalyzer(mainDictIs, affixDictIs, prefixDictIs)
        , sufDict(SuffixDict::loadSuffixDictFromStream(suffixDictIs))
    {
        const auto & suffix_map = dict->getSuffixMap();
        for (auto iter = suffix_map.begin(), iend = suffix_map.end(); iter != iend; ++iter)
            suffixesLength[iter->first] = iter->second.length();
    }

    SuffixDictAnalyzer();


    std::vector<ParsedPtr> analyze(const UniString & str) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & t) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const override;
    bool isDictWord(const UniString & str) const override { return true; }

    std::vector<ParsedPtr> generate(const UniString & str) const override;
};
}
