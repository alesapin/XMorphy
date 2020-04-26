#include "SuffixDictAnalyzer.h"
namespace X {
std::vector<ParsedPtr> SuffixDictAnalyzer::analyze(const utils::UniString& str) const {
    std::vector<ParsedPtr> r;
    if (PrefixAnalyzer::isDictWord(str)) {
        r = PrefixAnalyzer::analyze(str);
    }
    ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::vector<MorphDictInfo> result;
    dict->getClearForms(rawInfo, result);
    auto tmpRes = DictMorphAnalyzer::analyze(str, result);

    for (auto ptr : tmpRes) {
        ptr->at = AnalyzerTag::SUFF;
    }
    r.insert(r.end(), tmpRes.begin(), tmpRes.end());
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString& str, const UniMorphTag& t) const {
    if (PrefixAnalyzer::isDictWord(str)) {
        return PrefixAnalyzer::synthesize(str, t);
    }
    ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, t, result);
    for (auto ptr : r) {
        ptr->at = AnalyzerTag::SUFF;
    }
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString& str, const UniMorphTag& given, const UniMorphTag& req) const {
    if (PrefixAnalyzer::isDictWord(str)) {
        return PrefixAnalyzer::synthesize(str, given, req);
    }
    ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::map<Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, given, req, result);
    for (auto ptr : r) {
        ptr->at = AnalyzerTag::SUFF;
    }
    return r;
}
}
