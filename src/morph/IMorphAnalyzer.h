#ifndef _I_MORPH_ANALYZER_H
#define _I_MORPH_ANALYZER_H
#include <tag/UniMorphTag.h>
#include <tag/UniSPTag.h>
#include <utils/UniString.h>
#include <tag/AnalyzerTag.h>
#include <vector>
#include <memory>

namespace analyze {
struct Parsed {
	utils::UniString wordform;
	utils::UniString normalform;
	base::UniSPTag sp;
	base::UniMorphTag mt;
	base::AnalyzerTag at;
	std::size_t count;
    std::size_t stemLen;
};

using ParsedPtr = std::shared_ptr<Parsed>;

class IMorphAnalyzer {
public:
	virtual std::vector<ParsedPtr> analyze(const utils::UniString &str) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::UniMorphTag &t) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::UniMorphTag &given, const base::UniMorphTag &req) const = 0;
	virtual bool isDictWord(const utils::UniString &str) const = 0;
    virtual ~IMorphAnalyzer() {}
};

}
#endif
