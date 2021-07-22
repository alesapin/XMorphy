#include <xmorphy/build/MorphDict.h>

namespace X
{

std::vector<MorphDictInfo> MorphDict::getClearForms(const UniString & form) const
{
    std::string rawString = form.getRawString();
    std::vector<MorphDictInfo> result;
    if (mainDict->contains(rawString))
    {
        const ParaPairArray & paraCandidates = mainDict->getValue(rawString);
        getClearForms(paraCandidates, result);
    }
    return result;
}

void MorphDict::getClearForms(
    const ParaPairArray & paraCandidates, std::vector<MorphDictInfo> & result, const std::vector<size_t> & lengths) const
{
    result.reserve(paraCandidates.data.size());
    for (size_t i = 0; i < paraCandidates.data.size(); ++i)
    {
        const ParaPair & elem = paraCandidates.data[i];
        if (elem.paraNum >= paraMap.size())
            throw std::runtime_error(
                "Incorrect paradigm number " + std::to_string(elem.paraNum) + " largest is " + std::to_string(paraMap.size() - 1));

        const EncodedParadigm & encoded_paradigm = paraMap[elem.paraNum];
        const EncodedLexemeGroup & current = encoded_paradigm[elem.formNum];
        const EncodedLexemeGroup * normal = &encoded_paradigm[0];
        for (size_t i = elem.formNum; i != 0; --i)
        {
            if (encoded_paradigm[i].isNormalForm)
            {
                normal = &encoded_paradigm[i];
                break;
            }
        }

        MorphTagPair tp = tags.right.at(current.tagId);
        if (tp.sp == UniSPTag::X)
            throw std::runtime_error("Incorrect tag pair in binary dict for paradigm number " + std::to_string(elem.paraNum));
        const UniString & suffix = suffixes.at(current.suffixId);

        if (!lengths.empty() && suffix.length() > lengths[i])
            continue;

        const UniString & prefix = prefixes.at(current.prefixId);
        const UniString & nprefix = prefixes.at(normal->prefixId);
        const UniString & nsuffix = suffixes.at(normal->suffixId);
        LexemeGroup lg{prefix, tp.sp, tp.tag, suffix};
        AffixPair pair{nprefix, nsuffix};
        MorphDictInfo info{lg, pair, elem.freq};
        result.emplace_back(std::move(info));
    }
}

std::map<Paradigm, std::size_t> MorphDict::getParadigmsForForm(const UniString & form) const
{
    const std::string & rawString = form.getRawString();
    std::map<Paradigm, std::size_t> result;
    if (mainDict->contains(rawString))
    {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        getParadigmsForForm(paraCandidates, result);
    }
    return result;
}

void MorphDict::getParadigmsForForm(const ParaPairArray & paraCandidates, std::map<Paradigm, std::size_t> & result) const
{
    for (const ParaPair & elem : paraCandidates.data)
    {
        EncodedParadigm p = paraMap[elem.paraNum];
        Paradigm decoded = decodeParadigm(p);
        if (!result.count(decoded))
        {
            result[decoded] = elem.formNum;
        }
    }
}


void MorphDict::getParadigmsForFormWithFilter(const ParaPairArray & arr, std::map<Paradigm, std::size_t> & result, const std::vector<size_t> & length) const
{

    for (size_t i = 0; i < arr.data.size(); ++i)
    {
        const auto & elem = arr.data[i];
        EncodedParadigm p = paraMap[elem.paraNum];
        Paradigm decoded = decodeParadigm(p);
        bool foundLonger = false;
        for (const auto & lexemeGroup : decoded)
        {
            if (lexemeGroup.suffix.length() > length[i])
            {
                foundLonger = true;
                break;
            }
        }

        if (!result.count(decoded) && !foundLonger)
        {
            result[decoded] = elem.formNum;
        }
    }
}

Paradigm MorphDict::decodeParadigm(const EncodedParadigm & para) const
{
    Paradigm result(para.size());
    for (std::size_t i = 0; i < para.size(); ++i)
    {
        UniString prefix = prefixes.at(para[i].prefixId);
        MorphTagPair tp = tags.right.at(para[i].tagId);
        UniString suffix = suffixes.at(para[i].suffixId);
        result[i] = LexemeGroup{prefix, tp.sp, tp.tag, suffix, para[i].isNormalForm};
    }
    return result;
}

void dropToFiles(const std::unique_ptr<MorphDict> & dict, const std::string & mainDictFilename, const std::string & affixesFileName)
{
    std::ofstream ofsMain(mainDictFilename);
    std::ofstream ofsAffix(affixesFileName);
    dropMapToFile<UniString>(ofsAffix, dict->prefixes);
    dropBimapToFile<MorphTagPair>(ofsAffix, dict->tags);
    dropMapToFile<UniString>(ofsAffix, dict->suffixes);
    saveParas(dict->paraMap, ofsMain);
    dict->mainDict->serialize(ofsMain);
}

std::unique_ptr<MorphDict> MorphDict::loadFromFiles(std::istream & mainDictIs, std::istream & affixesIs)
{
    std::string row;
    std::unordered_map<size_t, UniString> prefixes, suffixes;
    boost::bimap<MorphTagPair, std::size_t> tags;
    readMapFromFile(affixesIs, prefixes);
    readBimapFromFile(affixesIs, tags);
    readMapFromFile(affixesIs, suffixes);
    DictPtr mainDict = std::make_shared<dawg::Dictionary<ParaPairArray>>();
    std::vector<EncodedParadigm> paras;
    loadParas(paras, mainDictIs);
    mainDict->deserialize(mainDictIs);
    return utils::make_unique<MorphDict>(paras, mainDict, prefixes, tags, suffixes);
}
}
