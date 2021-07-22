#pragma once
#include <unordered_set>
#include <xmorphy/morph/HyphenAnalyzer.h>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/utils/LRUCache.h>

namespace X
{
class Processor
{
private:
    mutable LRUCache<UniString, WordForm> forms_cache;
    std::shared_ptr<HyphenAnalyzer> morphAnalyzer;

private:
    WordFormPtr processOneToken(TokenPtr token) const;

    void parseWordLike(
        std::unordered_set<MorphInfo> & infos,
        const UniString & tokenString,
        const UniString & prefix,
        const UniString & postfix) const;

    void parseWordLike(
        std::unordered_set<MorphInfo> & infos,
        const UniString & tokenString) const;

    void parseNumbLike(std::unordered_set<MorphInfo> & infos, UniString && tokenString) const;

    void parseWordNumLike(std::unordered_set<MorphInfo> & infos, const UniString & tokenString) const;

public:
    Processor();
    std::vector<WordFormPtr> analyze(const std::vector<TokenPtr> & data) const;
    std::vector<TokenPtr> getNonDictionaryWords(const std::vector<TokenPtr> & data) const;
    WordFormPtr analyzeSingleToken(TokenPtr data) const;
    std::vector<WordFormPtr> synthesize(WordFormPtr form, UniMorphTag t) const;
    std::vector<WordFormPtr> synthesize(TokenPtr token, UniMorphTag t) const;
    std::vector<WordFormPtr> synthesize(const UniString & word, UniMorphTag t) const;
    bool isWordContainsInDictionary(const UniString & word) const;
    std::vector<ParsedPtr> generate(const UniString & str) const;
};
}
