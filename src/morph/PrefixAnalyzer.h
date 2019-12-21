#ifndef _PREFIX_ANALYZER_H
#define _PREFIX_ANALYZER_H
#include "DictMorphAnalyzer.h"
namespace analyze {
class PrefixAnalyzer : public DictMorphAnalyzer {
public:
	PrefixAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs, std::istream &prefixDictIs)
        : DictMorphAnalyzer(mainDictIs, affixDictIs)
        , prefDict(build::loadPrefixDict(prefixDictIs))
    {
	}

	std::vector<ParsedPtr> analyze(const utils::UniString &str) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &t) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &given, const base::MorphTag &req) const override;
	bool isDictWord(const utils::UniString &str) const override;
protected:
	std::set<utils::UniString> prefDict;
	std::set<utils::UniString> cutPrefix(const utils::UniString &source) const;
};
}
#endif
