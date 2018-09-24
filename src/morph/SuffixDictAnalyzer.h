#ifndef _SUFFIX_DICT_ANALYZER
#define _SUFFIX_DICT_ANALYZER
#include "PrefixAnalyzer.h"
#include <build/SuffixDict.h>
namespace analyze {
class SuffixDictAnalyzer : public PrefixAnalyzer {
protected:
	std::unique_ptr<build::SuffixDict> sufDict;
public:
	SuffixDictAnalyzer(const std::string & mainDictPath, const std::string &affixDictPath, const std::string &prefixDictPath, const std::string &suffixDictPath):
		PrefixAnalyzer(mainDictPath, affixDictPath, prefixDictPath) {
		loadFromFiles(sufDict, suffixDictPath);
	}
	std::vector<ParsedPtr> analyze(const utils::UniString &str) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &t) const override;
	std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &given, const base::MorphTag &req) const override;
	bool isDictWord(const utils::UniString &str) const override {return true;}

};
}
#endif
