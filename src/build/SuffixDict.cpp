#include "SuffixDict.h"
namespace build {
ParaPairArray SuffixDict::getCandidates(const utils::UniString& word) const {
    for (std::size_t i = std::min(word.length(), MAX_TAIL_LENGTH); i >= 1; --i) {
        std::string cut = word.rcut(i).getRawString();
        if (sd->contains(cut)) {
            return sd->getValue(cut);
        }
    }
    return ParaPairArray();
}

void dropToFiles(const std::unique_ptr<SuffixDict>& dict, const std::string& dictFilename) {
    std::ofstream ofsMain(dictFilename);
    saveParas(dict->paraMap, ofsMain);
    dict->sd->serialize(ofsMain);
}

std::unique_ptr<SuffixDict> SuffixDict::loadSuffixDictFromStream(std::istream & dictIs) {
    DictPtr mainDict = std::make_shared<dawg::Dictionary<ParaPairArray>>();
    std::vector<EncodedParadigm> paras;
    loadParas(paras, dictIs);
    mainDict->deserialize(dictIs);
    return utils::make_unique<SuffixDict>(paras, mainDict);
}
}
