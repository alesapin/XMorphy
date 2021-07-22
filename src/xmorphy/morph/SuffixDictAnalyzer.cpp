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
    const auto & suffix_map = dict->getSuffixMap();
    for (auto iter = suffix_map.begin(), iend = suffix_map.end(); iter != iend; ++iter)
        suffixesLength[iter->first] = iter->second.length();
}

std::vector<ParsedPtr> SuffixDictAnalyzer::analyze(const UniString & str) const
{
    if (PrefixAnalyzer::isDictWord(str))
        return PrefixAnalyzer::analyze(str);

    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths, suffixesLength);
    std::vector<MorphDictInfo> result;
    dict->getClearForms(rawInfo, result, lengths);
    auto tmpRes = DictMorphAnalyzer::analyze(str, result);

    for (auto & ptr : tmpRes)
        ptr->at = AnalyzerTag::SUFF;

    return tmpRes;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const UniString & str, const UniMorphTag & t) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::synthesize(str, t);
    }
    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths, suffixesLength);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, t, result);
    for (auto ptr : r)
        ptr->at = AnalyzerTag::SUFF;
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::synthesize(str, given, req);
    }
    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths, suffixesLength);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, given, req, result);
    for (auto ptr : r)
    {
        ptr->at = AnalyzerTag::SUFF;
    }
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::generate(const UniString & str) const
{
    if (PrefixAnalyzer::isDictWord(str))
    {
        return PrefixAnalyzer::generate(str);
    }

    std::vector<size_t> lengths;
    ParaPairArray rawInfo = sufDict->getCandidates(str, lengths, suffixesLength, true);

    std::map<Paradigm, std::size_t> paradigms;
    dict->getParadigmsForFormWithFilter(rawInfo, paradigms, lengths);

    /// Remove all paradigms where given form is not normal
    for (auto itr = paradigms.begin(); itr != paradigms.end();)
    {
        auto & lexeme_group = itr->first;
        size_t form_num = itr->second;
        if (!lexeme_group[form_num].isNormalForm)
            itr = paradigms.erase(itr);
        else
            ++itr;
    }

    auto result = DictMorphAnalyzer::generate(str, paradigms);
    for (auto ptr : result)
        ptr->at = AnalyzerTag::SUFF;
    return result;

}

}
