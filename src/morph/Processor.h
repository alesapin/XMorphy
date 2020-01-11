#ifndef _PROCESSOR_H
#define _PROCESSOR_H
#include "HyphenAnalyzer.h"
#include "WordForm.h"
#include <Resource.h>
namespace analyze {
class Processor {
private:
	std::shared_ptr<HyphenAnalyzer> morphAnalyzer;

private:
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
        : morphAnalyzer(
            std::make_shared<HyphenAnalyzer>(
                    mainDictIs,
                    affixDictIs,
                    prefixDictIs,
                    suffixDictIs,
                    hyphenAnalyzerIs
            ))
    {}

    Processor() {
        const auto& factory = CppResource::ResourceFactory::instance();
        std::istringstream mainIs(factory.getAsString("maindict"));
        std::istringstream affixIs(factory.getAsString("affixdict"));
        std::istringstream prefixDict(factory.getAsString("prefixdict"));
        std::istringstream suffixDict(factory.getAsString("suffixdict"));
        std::istringstream hyphDict(factory.getAsString("hyphdict"));
        morphAnalyzer = std::make_shared<HyphenAnalyzer>(mainIs, affixIs, prefixDict, suffixDict, hyphDict);
    }

    std::vector<WordFormPtr> analyze(const std::vector<base::TokenPtr> &data) const;
    WordFormPtr analyzeSingleToken(base::TokenPtr data) const;
	std::vector<WordFormPtr> synthesize(WordFormPtr form, base::UniMorphTag t) const;
	std::vector<WordFormPtr> synthesize(base::TokenPtr token, base::UniMorphTag t) const;
	std::vector<WordFormPtr> synthesize(const utils::UniString &word, base::UniMorphTag t) const;
};
}
#endif
