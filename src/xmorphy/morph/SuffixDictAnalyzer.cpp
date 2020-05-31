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

std::vector<ParsedPtr>
SuffixDictAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
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
}
