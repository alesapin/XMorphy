#pragma once
#include "HyphenAnalyzer.h"
#include "WordForm.h"
#include <unordered_set>

namespace X {
class Processor {
private:
	std::shared_ptr<HyphenAnalyzer> morphAnalyzer;

private:
	WordFormPtr processOneToken(TokenPtr token) const;

	TokenPtr joinHyphenGroup(std::size_t &index, const std::vector<TokenPtr> &data) const;

	void parseWordLike(std::unordered_set<MorphInfo> &infos, const utils::UniString &tokenString, const utils::UniString &prefix = utils::UniString(""), const utils::UniString &postfix = utils::UniString("")) const;

	void parseNumbLike(std::unordered_set<MorphInfo> &infos, const utils::UniString &tokenString) const;

	void parseWordNumLike(std::unordered_set<MorphInfo> &infos, const utils::UniString &tokenString) const;

public:
	Processor(
            std::istream & mainDictIs,
            std::istream & affixDictIs,
            std::istream & prefixDictIs,
            std::istream & suffixDictIs,
            std::istream & hyphenAnalyzerIs)
        : morphAnalyzer(
            std::make_shared<HyphenAnalyzer>(
                    mainDictIs,
                    affixDictIs,
                    prefixDictIs,
                    suffixDictIs,
                    hyphenAnalyzerIs
            ))
    {}

    Processor();
    std::vector<WordFormPtr> analyze(const std::vector<TokenPtr> &data) const;
    WordFormPtr analyzeSingleToken(TokenPtr data) const;
	std::vector<WordFormPtr> synthesize(WordFormPtr form, UniMorphTag t) const;
	std::vector<WordFormPtr> synthesize(TokenPtr token, UniMorphTag t) const;
	std::vector<WordFormPtr> synthesize(const utils::UniString &word, UniMorphTag t) const;
};
}
