#pragma once
#include "PrefixAnalyzer.h"
#include <build/SuffixDict.h>
namespace X {
class SuffixDictAnalyzer : public PrefixAnalyzer {
protected:
	std::unique_ptr<SuffixDict> sufDict;
public:
	SuffixDictAnalyzer(
            std::istream & mainDictIs,
            std::istream & affixDictIs,
            std::istream & prefixDictIs,
            std::istream & suffixDictIs)
        : PrefixAnalyzer(mainDictIs, affixDictIs, prefixDictIs)
        , sufDict(SuffixDict::loadSuffixDictFromStream(suffixDictIs))
    {
	}

	std::vector<ParsedPtr> analyze(const utils::UniString & str) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & t) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag &req) const override;
	bool isDictWord(const utils::UniString &str) const override {return true;}

};
}
