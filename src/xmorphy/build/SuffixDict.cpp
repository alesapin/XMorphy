#include <xmorphy/build/SuffixDict.h>

namespace X
{
ParaPairArray SuffixDict::getCandidates(const UniString & word, std::vector<size_t> & tailsLens, const std::unordered_map<size_t, size_t> & suffixes_length, bool longest) const
{
    ParaPairArray result;
    std::unordered_map<size_t, size_t> paraCounter;
    for (std::size_t i = std::max(1UL, std::min(word.length() - 1, MAX_TAIL_LENGTH)); i >= 1; --i)
    {
        if (i < 3 && !result.data.empty())
            break;

        std::string cut = word.rcut(i).getRawString();
        if (sd->contains(cut))
        {
            ParaPairArray value = sd->getValue(cut);
            for (const ParaPair pair : value.data)
            {
                const EncodedParadigm & encoded_paradigm = paraMap[pair.paraNum];
                const EncodedLexemeGroup & current = encoded_paradigm[pair.formNum];
                if (suffixes_length.at(current.suffixId) > i)
                    continue;

                if (result.data.size() > MAX_FORMS_TOTAL)
                    return result;

                if (paraCounter.count(pair.paraNum) < MAX_FORMS_IN_ONE_PARA)
                {
                    tailsLens.push_back(i);
                    result.data.push_back(pair);
                    paraCounter[pair.paraNum]++;
                }
            }
            if (longest)
                break;
        }
    }
    return result;
}

void dropToFiles(const std::unique_ptr<SuffixDict> & dict, const std::string & dictFilename)
{
    std::ofstream ofsMain(dictFilename);
    saveParas(dict->paraMap, ofsMain);
    dict->sd->serialize(ofsMain);
}

std::unique_ptr<SuffixDict> SuffixDict::loadSuffixDictFromStream(std::istream & dictIs)
{
    DictPtr mainDict = std::make_shared<dawg::Dictionary<ParaPairArray>>();
    std::vector<EncodedParadigm> paras;
    loadParas(paras, dictIs);
    mainDict->deserialize(dictIs);
    return utils::make_unique<SuffixDict>(paras, mainDict);
}
}
