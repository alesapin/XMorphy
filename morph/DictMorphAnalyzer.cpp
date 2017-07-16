#include "DictMorphAnalyzer.h"
namespace analyze {

DictMorphAnalyzer::DictMorphAnalyzer(const std::string& mainDictPath, const std::string& affixDictPath) {
    loadFromFiles(this->dict, mainDictPath, affixDictPath);
}

utils::UniString DictMorphAnalyzer::buildNormalForm(utils::UniString wordForm,
                                                    utils::UniString formPrefix,
                                                    utils::UniString formSuffix,
                                                    utils::UniString normalFormPrefix,
                                                    utils::UniString normalFormSuffix) const {

    if (formPrefix.length() < wordForm.length()) {
        if (!formPrefix.isEmpty()) {
            wordForm = wordForm.subString(formPrefix.length());
        }
    } else {
        normalFormPrefix = utils::UniString("");
    }
    utils::UniString stem = wordForm.subString(0, wordForm.length() - formSuffix.length());;
    return normalFormPrefix + stem + normalFormSuffix;
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const utils::UniString& str) const {
    std::vector<std::tuple<build::LexemeGroup, build::AffixPair, std::size_t>> dictInfo = dict->getClearForms(str);
    return analyze(str, dictInfo);
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const utils::UniString& str, const std::vector<std::tuple<build::LexemeGroup, build::AffixPair, std::size_t>>& dictInfo) const {
    std::vector<ParsedPtr> result(dictInfo.size());
    std::size_t i = 0;
    for (auto& itr : dictInfo) {
        base::SpeechPartTag spt;
        base::MorphTag mt;
        utils::UniString prefix, suffix, nprefix, nsuffix;
        std::tie(prefix, spt, mt, suffix) = std::get<0>(itr);
        std::tie(nprefix, nsuffix) = std::get<1>(itr);
        utils::UniString normalForm = buildNormalForm(str, prefix, suffix, nprefix, nsuffix);
        result[i] = std::make_shared<Parsed>(Parsed{str, normalForm, spt, mt, base::AnalyzerTag::DICT, std::get<2>(itr), normalForm.length() - nsuffix.length()});
        i++;
    }
    return result;
}

ParsedPtr DictMorphAnalyzer::buildByPara(const build::LexemeGroup& reqForm, const build::LexemeGroup& givenForm, const build::LexemeGroup& normalForm, const utils::UniString& given) const {
    base::SpeechPartTag spt;
    base::MorphTag mt;
    utils::UniString prefix, suffix, nprefix, nsuffix, reqPrefix, reqSuffix;
    std::tie(prefix, spt, std::ignore, suffix) = givenForm;
    std::tie(std::ignore, std::ignore, mt, std::ignore) = reqForm;
    nprefix = std::get<0>(normalForm);
    nsuffix = std::get<3>(normalForm);
    reqPrefix = std::get<0>(reqForm);
    reqSuffix = std::get<3>(reqForm);
    utils::UniString nF = buildNormalForm(given, prefix, suffix, nprefix, nsuffix);
    utils::UniString f = buildNormalForm(given, prefix, suffix, reqPrefix, reqSuffix);
    return std::make_shared<Parsed>(Parsed{f, nF, spt, mt, base::AnalyzerTag::DICT, 0, nF.length() - nsuffix.length()});
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString& str, const base::MorphTag& t) const {
    std::map<build::Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, t, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString& str, const base::MorphTag& t, const std::map<build::Paradigm, std::size_t>& paras) const {
    std::vector<ParsedPtr> result;
    for (const auto& para : paras) {
        build::LexemeGroup given = para.first[para.second];
        for (const build::LexemeGroup& group : para.first) {
            base::MorphTag current = std::get<2>(group);
            if (current.contains(t)) {
                result.push_back(buildByPara(group, given, para.first[0], str));
            }
        }
    }
    return result;
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString& str, const base::MorphTag& given, const base::MorphTag& req) const {
    std::map<build::Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, given, req, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString& str, const base::MorphTag& given, const base::MorphTag& req, const std::map<build::Paradigm, std::size_t>& paras) const {
    std::vector<ParsedPtr> result;
    for (const auto& para : paras) {
        build::LexemeGroup lg = para.first[para.second];
        for (const build::LexemeGroup& group : para.first) {
            base::MorphTag current = std::get<2>(group);
            if (current.resetIfContains(req) && given.contains(current)) { //Tag contains all required, and rest tags are given
                result.push_back(buildByPara(group, lg, para.first[0], str));
            }
        }
    }
    return result;
}

bool DictMorphAnalyzer::isDictWord(const utils::UniString& str) const {
    return dict->contains(str);
}
}
