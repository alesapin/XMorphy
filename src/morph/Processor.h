#ifndef _PROCESSOR_H
#define _PROCESSOR_H
#include "HyphenAnalyzer.h"
#include "WordForm.h"
namespace analyze {
class Processor {
private:
	std::shared_ptr<HyphenAnalyzer> morphAnalyzer;
	WordFormPtr processOneToken(base::TokenPtr token) const;
	base::TokenPtr joinHyphenGroup(std::size_t &index, const std::vector<base::TokenPtr> &data) const;
	void parseWordLike(std::set<MorphInfo> &infos, const utils::UniString &tokenString, const utils::UniString &prefix = utils::UniString(""), const utils::UniString &postfix = utils::UniString("")) const;
	void parseNumbLike(std::set<MorphInfo> &infos, const utils::UniString &tokenString) const;
	void parseWordNumLike(std::set<MorphInfo> &infos, const utils::UniString &tokenString) const;
public:
	Processor(
            std::istream & mainDictIs,
            std::istream & affixDictIs,
            std::istream & prefixDictIs,
            std::istream & suffixDictIs,
            std::istream & hyphenAnalyzerIs)
        : morphAnalyzer(std::make_shared<HyphenAnalyzer>(
                    mainDictIs, affixDictIs, prefixDictIs, suffixDictIs, hyphenAnalyzerIs)) 
    {}

    std::vector<WordFormPtr> analyze(const std::vector<base::TokenPtr> &data) const;
    WordFormPtr analyzeSingleToken(base::TokenPtr data) const;
	std::vector<WordFormPtr> synthesize(WordFormPtr form, base::MorphTag t) const;
	std::vector<WordFormPtr> synthesize(base::TokenPtr token, base::MorphTag t) const;
	std::vector<WordFormPtr> synthesize(const utils::UniString &word, base::MorphTag t) const;
};
}
#endif
