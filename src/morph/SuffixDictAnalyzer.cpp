#include "SuffixDictAnalyzer.h"
namespace analyze {
std::vector<ParsedPtr> SuffixDictAnalyzer::analyze(const utils::UniString& str) const {
    std::vector<ParsedPtr> r;
    if (PrefixAnalyzer::isDictWord(str)) {
        r = PrefixAnalyzer::analyze(str);
    }
    build::ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::vector<build::MorphDictInfo> result;
    dict->getClearForms(rawInfo, result);
    auto tmpRes = DictMorphAnalyzer::analyze(str, result);

    for (auto ptr : tmpRes) {
        ptr->at = base::AnalyzerTag::SUFF;
    }
    r.insert(r.end(), tmpRes.begin(), tmpRes.end());
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString& str, const base::UniMorphTag& t) const {
    if (PrefixAnalyzer::isDictWord(str)) {
        return PrefixAnalyzer::synthesize(str, t);
    }
    build::ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::map<build::Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, t, result);
    for (auto ptr : r) {
        ptr->at = base::AnalyzerTag::SUFF;
    }
    return r;
}

std::vector<ParsedPtr> SuffixDictAnalyzer::synthesize(const utils::UniString& str, const base::UniMorphTag& given, const base::UniMorphTag& req) const {
    if (PrefixAnalyzer::isDictWord(str)) {
        return PrefixAnalyzer::synthesize(str, given, req);
    }
    build::ParaPairArray rawInfo = sufDict->getCandidates(str);
    std::map<build::Paradigm, std::size_t> result;
    dict->getParadigmsForForm(rawInfo, result);
    std::vector<ParsedPtr> r = DictMorphAnalyzer::synthesize(str, given, req, result);
    for (auto ptr : r) {
        ptr->at = base::AnalyzerTag::SUFF;
    }
    return r;
}
}
