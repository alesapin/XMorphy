#pragma once
#include <xmorphy/build/DictBuilder.h>
#include <xmorphy/morph/SuffixDictAnalyzer.h>
namespace X
{
class HyphenAnalyzer : public SuffixDictAnalyzer
{
private:
    std::vector<ParsedPtr> twoParsesAnalyze(const std::vector<ParsedPtr> & left, const std::vector<ParsedPtr> & right) const;
    std::vector<ParsedPtr> twoWordsAnalyze(const UniString & first, const UniString & second) const;
    PrefixDict constParts;

public:
    HyphenAnalyzer(
        std::istream & mainDictIs,
        std::istream & affixDictIs,
        std::istream & prefixDictIs,
        std::istream & suffixDictIs,
        std::istream & hyphenDictIs)
        : SuffixDictAnalyzer(mainDictIs, affixDictIs, prefixDictIs, suffixDictIs)
        , constParts(loadPrefixDict(hyphenDictIs))
    {
    }

    HyphenAnalyzer();

    using SuffixDictAnalyzer::isDictWord;
    std::vector<ParsedPtr> analyze(const UniString & str) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & t) const override;
    std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const override;
};
} // namespace analyze
