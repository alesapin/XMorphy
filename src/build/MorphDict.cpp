#include "MorphDict.h"
namespace build {

std::vector<LexemeGroup> MorphDict::getForms(const utils::UniString& form) const {
    const std::string& rawString = form.getRawString();
    std::vector<LexemeGroup> result;
    if (mainDict->contains(rawString)) {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        for (const ParaPair& elem : paraCandidates.data) {
            EncodedParadigm p = paraMap[elem.paraNum];
            EncodedLexemeGroup g = p[elem.formNum];
            utils::UniString prefix = prefixes.right.at(std::get<0>(g));
            TagPair ts = tags.right.at(std::get<1>(g));
            utils::UniString suffix = suffixes.right.at(std::get<2>(g));
            result.push_back(std::make_tuple(prefix, ts.first, ts.second, suffix));
        }
    }
    return result;
}

std::vector<std::tuple<LexemeGroup, AffixPair, std::size_t>> MorphDict::getClearForms(const utils::UniString& form) const {
    const std::string& rawString = form.getRawString();
    std::vector<std::tuple<LexemeGroup, AffixPair, std::size_t>> result;
    if (mainDict->contains(rawString)) {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        getClearForms(paraCandidates, result);
    }
    return result;
}

void MorphDict::getClearForms(const ParaPairArray& paraCandidates, std::vector<std::tuple<LexemeGroup, AffixPair, std::size_t>>& result) const {
    for (const ParaPair& elem : paraCandidates.data) {
        if (elem.paraNum >= paraMap.size())
            throw std::runtime_error("Incorrect paradigm number " + std::to_string(elem.paraNum) + " largest is " + std::to_string(paraMap.size() - 1));

        EncodedParadigm p = paraMap[elem.paraNum];
        EncodedLexemeGroup current = p[elem.formNum];
        EncodedLexemeGroup normal = p[0];
        TagPair tp = tags.right.at(std::get<1>(current));
        if (std::get<0>(tp) == base::UniSPTag::X)
            throw std::runtime_error("Incorrect tag pair in binary dict for paradigm number " + std::to_string(elem.paraNum));
        utils::UniString prefix = prefixes.right.at(std::get<0>(current));
        utils::UniString suffix = suffixes.right.at(std::get<2>(current));
        utils::UniString nprefix = prefixes.right.at(std::get<0>(normal));
        utils::UniString nsuffix = suffixes.right.at(std::get<2>(normal));
        result.push_back(std::make_tuple(std::make_tuple(prefix, tp.first, tp.second, suffix), std::make_pair(nprefix, nsuffix), elem.freq));
    }
}

std::map<Paradigm, std::size_t> MorphDict::getParadigmsForForm(const utils::UniString& form) const {
    const std::string& rawString = form.getRawString();
    std::map<Paradigm, std::size_t> result;
    if (mainDict->contains(rawString)) {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        getParadigmsForForm(paraCandidates, result);
    }
    return result;
}

void MorphDict::getParadigmsForForm(const ParaPairArray& paraCandidates, std::map<Paradigm, std::size_t>& result) const {
    for (const ParaPair& elem : paraCandidates.data) {
        EncodedParadigm p = paraMap[elem.paraNum];
        Paradigm decoded = decodeParadigm(p);
        if (!result.count(decoded)) {
            result[decoded] = elem.formNum;
        }
    }
}

Paradigm MorphDict::decodeParadigm(const EncodedParadigm& para) const {
    Paradigm result(para.size());
    for (std::size_t i = 0; i < para.size(); ++i) {
        std::size_t prefixId, tagId, suffixId;
        std::tie(prefixId, tagId, suffixId) = para[i];
        utils::UniString prefix = prefixes.right.at(prefixId);
        TagPair tp = tags.right.at(tagId);
        utils::UniString suffix = suffixes.right.at(suffixId);
        result[i] = std::make_tuple(prefix, tp.first, tp.second, suffix);
    }
    return result;
}

void dropToFiles(const std::unique_ptr<MorphDict>& dict, const std::string& mainDictFilename, const std::string& affixesFileName) {
    std::ofstream ofsMain(mainDictFilename);
    std::ofstream ofsAffix(affixesFileName);
    dropBimapToFile<utils::UniString>(ofsAffix, dict->prefixes);
    dropBimapToFile<TagPair>(ofsAffix, dict->tags);
    dropBimapToFile<utils::UniString>(ofsAffix, dict->suffixes);
    saveParas(dict->paraMap, ofsMain);
    dict->mainDict->serialize(ofsMain);
}

std::unique_ptr<MorphDict> MorphDict::loadFromFiles(std::istream & mainDictIs, std::istream & affixesIs) {
    std::string row;
    boost::bimap<utils::UniString, std::size_t> prefixes, suffixes;
    boost::bimap<TagPair, std::size_t> tags;
    readBimapFromFile(affixesIs, prefixes);
    readBimapFromFile(affixesIs, tags);
    readBimapFromFile(affixesIs, suffixes);
    DictPtr mainDict = std::make_shared<dawg::Dictionary<ParaPairArray>>();
    std::vector<EncodedParadigm> paras;
    loadParas(paras, mainDictIs);
    mainDict->deserialize(mainDictIs);
    return utils::make_unique<MorphDict>(paras, mainDict, prefixes, tags, suffixes);
}
}
