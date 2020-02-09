#ifndef _HYPHEN_ANALYZER_H
#define _HYPHEN_ANALYZER_H
#include <build/DictBuilder.h>
#include "SuffixDictAnalyzer.h"
namespace analyze {
class HyphenAnalyzer : public SuffixDictAnalyzer {
private:
    std::vector<ParsedPtr>
    twoParsesAnalyze(const std::vector<ParsedPtr> &left,
                     const std::vector<ParsedPtr> &right) const;
    std::vector<ParsedPtr> twoWordsAnalyze(const utils::UniString &first,
                                           const utils::UniString &second) const;
    std::set<utils::UniString> constParts;

public:
    HyphenAnalyzer(
        std::istream & mainDictIs,
        std::istream & affixDictIs,
        std::istream & prefixDictIs,
        std::istream & suffixDictIs,
        std::istream & hyphenDictIs)
        : SuffixDictAnalyzer(mainDictIs, affixDictIs, prefixDictIs, suffixDictIs) 
        , constParts(build::loadPrefixDict(hyphenDictIs))
    {
    }
    using SuffixDictAnalyzer::isDictWord;
    std::vector<ParsedPtr> analyze(const utils::UniString &str) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString &str,
                                      const base::UniMorphTag &t) const override;
    std::vector<ParsedPtr> synthesize(const utils::UniString &str,
                                      const base::UniMorphTag &given,
                                      const base::UniMorphTag &req) const override;
};
} // namespace analyze
#endif
