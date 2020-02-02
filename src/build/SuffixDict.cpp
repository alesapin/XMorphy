#include "SuffixDict.h"
namespace build {
ParaPairArray SuffixDict::getCandidates(const utils::UniString& word) const {
    ParaPairArray result;
    std::unordered_map<size_t, size_t> paraCounter;
    for (std::size_t i = std::min(word.length(), MAX_TAIL_LENGTH); i >= 1; --i) {
        if (i < 3 && !result.data.empty())
            break;
        std::string cut = word.rcut(i).getRawString();
        if (sd->contains(cut)) {
            ParaPairArray value = sd->getValue(cut);
            for (const ParaPair pair : value.data) {
                if (result.data.size() > MAX_FORMS_TOTAL)
                    return result;
                if (paraCounter.count(pair.paraNum) < MAX_FORMS_IN_ONE_PARA)
                {
                    result.data.push_back(pair);
                    paraCounter[pair.paraNum]++;
                }

            }
        }
    }
    return result;
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
