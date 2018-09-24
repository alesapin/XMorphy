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
	Processor(const std::string & mainDictPath, const std::string &affixDictPath, const std::string &prefixDictPath, const std::string &suffixDictPath, const std::string &hyphenAnalyzer):
		morphAnalyzer(new HyphenAnalyzer(mainDictPath, affixDictPath, prefixDictPath, suffixDictPath, hyphenAnalyzer)) {}

    std::vector<WordFormPtr> analyze(const std::vector<base::TokenPtr> &data) const;
	std::vector<WordFormPtr> synthesize(WordFormPtr form, base::MorphTag t) const;
	std::vector<WordFormPtr> synthesize(base::TokenPtr token, base::MorphTag t) const;
	std::vector<WordFormPtr> synthesize(const utils::UniString &word, base::MorphTag t) const;
};
}
#endif
