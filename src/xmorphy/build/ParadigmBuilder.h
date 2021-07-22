#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <boost/algorithm/string.hpp>
#include <xmorphy/build/BuildDefs.h>
#include <xmorphy/build/RawDict.h>
namespace X
{
using StringToIndexBiMap = boost::bimap<UniString, std::size_t>;
using TagToIndexBiMap = boost::bimap<MorphTagPair, std::size_t>;

struct ParadigmOccurences
{
    size_t paradigmNumber;
    size_t paradigmFrequency;
};

struct IntermediateParadigmsState
{
    StringToIndexBiMap prefixesMap;
    TagToIndexBiMap tagsMap;
    StringToIndexBiMap suffixesMap;
};

class ParadigmBuilder
{
private:
    std::size_t freqThreshold;

public:
    ParadigmBuilder(std::size_t paradigmFreqThreshold = 1) : freqThreshold(paradigmFreqThreshold) {}
    std::map<Paradigm, ParadigmOccurences> getParadigms(const RawDict & rd) const;
};

IntermediateParadigmsState splitParadigms(const std::map<Paradigm, ParadigmOccurences> & paras);

std::map<EncodedParadigm, std::size_t> encodeParadigms(const std::map<Paradigm, ParadigmOccurences> & paras, const IntermediateParadigmsState & intermediateState);

Paradigm parseOnePara(const WordsArray & words, const TagsArray & tags, const std::vector<bool> & nf_bits);
inline std::ostream & operator<<(std::ostream & os, MorphTagPair p)
{
    return os << p.sp << " " << p.tag;
}

inline std::istream & operator>>(std::istream & is, MorphTagPair & p)
{
    return is >> p.sp >> p.tag;
}

template <typename T>
std::ostream & dropBimapToFile(std::ostream & os, const boost::bimap<T, std::size_t> & m)
{
    os << m.size() << "\n";
    for (auto iter = m.begin(), iend = m.end(); iter != iend; ++iter)
    {
        os << iter->left << "\t" << iter->right << "\n";
    }
    return os;
}

template <typename T>
std::ostream & dropMapToFile(std::ostream & os, const std::unordered_map<std::size_t, T> & m)
{
    os << m.size() << "\n";
    for (auto iter = m.begin(), iend = m.end(); iter != iend; ++iter)
    {
        os << iter->second << "\t" << iter->first << "\n";
    }
    return os;
}

void readBimapFromFile(std::istream & is, boost::bimap<UniString, std::size_t> & m);
void readMapFromFile(std::istream & is, std::unordered_map<size_t, UniString> & m);

void readBimapFromFile(std::istream & is, boost::bimap<MorphTagPair, std::size_t> & m);

}
