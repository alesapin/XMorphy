#pragma once
#include <tag/UniMorphTag.h>
#include <tag/UniSPTag.h>
#include <utils/UniString.h>
#include <tag/AnalyzerTag.h>
#include <vector>
#include <memory>

namespace X {
struct Parsed {
	utils::UniString wordform;
	utils::UniString normalform;
	UniSPTag sp;
	UniMorphTag mt;
	AnalyzerTag at;
	std::size_t count;
    std::size_t stemLen;
};

using ParsedPtr = std::shared_ptr<Parsed>;

class IMorphAnalyzer {
public:
	virtual std::vector<ParsedPtr> analyze(const utils::UniString &str) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const UniMorphTag &t) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const UniMorphTag &given, const UniMorphTag &req) const = 0;
	virtual bool isDictWord(const utils::UniString &str) const = 0;
    virtual ~IMorphAnalyzer() {}
};

}
