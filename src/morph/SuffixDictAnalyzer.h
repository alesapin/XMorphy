#ifndef _SUFFIX_DICT_ANALYZER
#define _SUFFIX_DICT_ANALYZER
#include "PrefixAnalyzer.h"
#include <build/SuffixDict.h>
namespace analyze {
class SuffixDictAnalyzer : public PrefixAnalyzer {
protected:
	std::unique_ptr<build::SuffixDict> sufDict;
public:
	SuffixDictAnalyzer(
            std::istream & mainDictIs,
            std::istream & affixDictIs,
            std::istream & prefixDictIs,
            std::istream & suffixDictIs)
        : PrefixAnalyzer(mainDictIs, affixDictIs, prefixDictIs)
        , sufDict(build::SuffixDict::loadSuffixDictFromStream(suffixDictIs))
    {
	}

	std::vector<ParsedPtr> analyze(const utils::UniString & str) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString & str, const base::MorphTag & t) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString & str, const base::MorphTag & given, const base::MorphTag &req) const override;
	bool isDictWord(const utils::UniString &str) const override {return true;}

};
}
#endif
