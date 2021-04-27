#pragma once
#include <memory>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/build/BuildDefs.h>
#include <xmorphy/build/ParadigmBuilder.h>
#include <unordered_map>
namespace X
{
static constexpr size_t MAX_TAIL_LENGTH = 5;
static constexpr size_t MAX_FORMS_IN_ONE_PARA = 3;
static constexpr size_t MAX_FORMS_TOTAL = 50;
class SuffixDict
{
private:
    DictPtr sd;
    std::vector<EncodedParadigm> paraMap;

public:
    SuffixDict(const std::vector<EncodedParadigm> & paraMap, DictPtr suffixDict) : sd(suffixDict), paraMap(paraMap) {}

    ParaPairArray getCandidates(const UniString & word, std::vector<size_t> & tailsLens, const std::unordered_map<size_t, size_t> & suffixes_length, bool longest = false) const;
    friend void dropToFiles(const std::unique_ptr<SuffixDict> & dict, const std::string & dictFilename);
    static std::unique_ptr<SuffixDict> loadSuffixDictFromStream(std::istream & dictIs);
};
}
