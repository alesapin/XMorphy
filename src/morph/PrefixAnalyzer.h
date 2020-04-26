#pragma once
#include "DictMorphAnalyzer.h"
namespace X {
class PrefixAnalyzer : public DictMorphAnalyzer {
public:
	PrefixAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs, std::istream &prefixDictIs)
        : DictMorphAnalyzer(mainDictIs, affixDictIs)
        , prefDict(loadPrefixDict(prefixDictIs))
    {
	}

	std::vector<ParsedPtr> analyze(const utils::UniString &str) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const UniMorphTag &t) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const UniMorphTag &given, const UniMorphTag &req) const override;
	bool isDictWord(const utils::UniString &str) const override;
protected:
	std::set<utils::UniString> prefDict;
	std::set<utils::UniString> cutPrefix(const utils::UniString &source) const;
};
}
