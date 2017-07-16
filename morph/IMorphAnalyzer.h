#ifndef _I_MORPH_ANALYZER_H
#define _I_MORPH_ANALYZER_H
#include "../tag/MorphTag.h"
#include "../tag/SpeechPartTag.h"
#include "../utils/UniString.h"
#include "../tag/AnalyzerTag.h"
#include <vector>
#include <memory>

namespace analyze {
struct Parsed {
	utils::UniString wordform;
	utils::UniString normalform;
	base::SpeechPartTag sp;
	base::MorphTag mt;
	base::AnalyzerTag at;
	std::size_t count;
    std::size_t stemLen;
};

using ParsedPtr = std::shared_ptr<Parsed>;

class IMorphAnalyzer {
public:
	virtual std::vector<ParsedPtr> analyze(const utils::UniString &str) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &t) const = 0;
	virtual std::vector<ParsedPtr> synthesize(const utils::UniString &str, const base::MorphTag &given, const base::MorphTag &req) const = 0;
	virtual bool isDictWord(const utils::UniString &str) const = 0;
};

}
#endif
