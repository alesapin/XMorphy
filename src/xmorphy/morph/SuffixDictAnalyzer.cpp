#include <xmorphy/morph/SuffixDictAnalyzer.h>

#include <incbin.h>

namespace X
{
namespace
{

INCBIN(suffixdict, "dicts/udsuffixdict.bin");

}

SuffixDictAnalyzer::SuffixDictAnalyzer()
{
    std::istringstream suffixDictIs(std::string{reinterpret_cast<const char *>(gsuffixdictData), gsuffixdictSize});
    sufDict = SuffixDict::loadSuffixDictFromStream(suffixDictIs);
}

std::vector<ParsedPtr> SuffixDictAnalyzer::analyze(const utils::UniString & str) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::analyze(str);
    }

    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths);
    std::vector<MorphDictInfo> result;
    dict->getClearForms(rawInfo, result, lengths);
    auto tmpRes = DictMorphAnalyzer::analyze(str, result);

    for (auto & ptr : tmpRes)
        ptr->at = AnalyzerTag::SUFF;

    return tmpRes;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & t) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::synthesize(str, t);
    }
    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, t, result);
    for (auto ptr : r)
        ptr->at = AnalyzerTag::SUFF;
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::synthesize(str, given, req);
    }
    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, given, req, result);
    for (auto ptr : r)
    {
        ptr->at = AnalyzerTag::SUFF;
    }
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::generate(const utils::UniString & str) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::generate(str);
    }

    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths);

    std::map<Paradigm, std::size_t> paradigms;
    dict->getParadigmsForForm(rawInfo, paradigms);
    std::map<size_t, size_t> top_frequent;
    for (auto itr = paradigms.begin(); itr != paradigms.end(); ++itr)
    {
        top_frequent[itr->second] += 1;
        if (top_frequent.size() > 1)
            top_frequent.erase(top_frequent.begin());
    }

    for (auto itr = paradigms.begin(); itr != paradigms.end();)
    {
        if (!top_frequent.count(itr->second))
            itr = paradigms.erase(itr);
        else
            ++itr;
    }

    while (paradigms.size() > 1)
        paradigms.erase(paradigms.rbegin()->first);

    auto result = DictMorphAnalyzer::generate(str, paradigms);
    for (auto ptr : result)
        ptr->at = AnalyzerTag::SUFF;
    return result;

}

}
