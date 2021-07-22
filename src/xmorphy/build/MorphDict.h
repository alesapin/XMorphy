#pragma once
#include <memory>
#include <xmorphy/DAWG/BuildFactory.h>
#include <xmorphy/DAWG/Dictionary.h>
#include <boost/algorithm/string.hpp>
#include <boost/bimap.hpp>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/build/BuildDefs.h>
#include <xmorphy/build/ParadigmBuilder.h>
namespace X
{

struct MorphDictInfo
{
    LexemeGroup lexemeGroup;
    AffixPair affixPair;
    size_t occurences;
};

class MorphDict
{
public:
    MorphDict(
        const std::vector<EncodedParadigm> & paraMap,
        DictPtr mainDict,
        const std::unordered_map<size_t, UniString> & prefs,
        const TagToIndexBiMap & tags,
        const std::unordered_map<size_t, UniString> & sufs)
        : paraMap(paraMap)
        , mainDict(mainDict)
        , prefixes(prefs)
        , tags(tags)
        , suffixes(sufs)
    {
    }

    std::vector<MorphDictInfo> getClearForms(const UniString & form) const;
    std::map<Paradigm, std::size_t> getParadigmsForForm(const UniString & form) const;
    void getClearForms(const ParaPairArray & arr, std::vector<MorphDictInfo> & result, const std::vector<size_t> & lengths = {}) const;
    void getParadigmsForForm(const ParaPairArray & arr, std::map<Paradigm, std::size_t> & result) const;

    void getParadigmsForFormWithFilter(const ParaPairArray & arr, std::map<Paradigm, std::size_t> & result, const std::vector<size_t> & length) const;

    bool contains(const UniString & form) const { return mainDict->contains(form.getRawString()); }
    friend void dropToFiles(const std::unique_ptr<MorphDict> & dict, const std::string & mainDictFilename, const std::string & affixesFileName);
    static std::unique_ptr<MorphDict> loadFromFiles(std::istream & mainDictIs, std::istream & affixesIs);

    const std::unordered_map<size_t, UniString> & getSuffixMap() const
    {
        return suffixes;
    }

private:
    std::vector<EncodedParadigm> paraMap;
    DictPtr mainDict;
    std::unordered_map<size_t, UniString> prefixes;
    TagToIndexBiMap tags;
    std::unordered_map<size_t, UniString> suffixes;

    Paradigm decodeParadigm(const EncodedParadigm & para) const;
};

}
