#pragma once
#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>
#include <boost/algorithm/string.hpp>

#include <xmorphy/build/BuildDefs.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/utils/UniString.h>

namespace X
{
using WordsArray = std::vector<UniString>;
using TagsArray = std::vector<MorphTagPair>;

struct WordsWithTags
{
    WordsArray words;
    TagsArray tags;
    std::vector<bool> nf_mask;
};

using RawArray = std::vector<WordsWithTags>;
using LemataMap = std::vector<std::optional<WordsWithTags>>;

template <typename SP, typename MT>
std::tuple<SP, MT> getTags(const std::string & str)
{
    std::vector<std::string> tgs;
    boost::split(tgs, str, boost::is_any_of(",|"));

    SP resultSP = SP(tgs[0]);

    tgs.erase(tgs.begin());
    MT resultTag = MT::UNKN;
    for (const auto & tg : tgs)
    {
        try
        {
            resultTag |= MT(tg);
        }
        catch (const std::out_of_range & e)
        {
            //std::cerr << "No such tag:" << tg << "\n";
        }
    }
    return std::make_tuple(resultSP, resultTag);
}

class RawDict
{
private:
    RawArray data;
    std::string filepath;

    RawDict(RawArray && data_, const std::string & filepath_) : data(std::move(data_)), filepath(filepath_) {}

public:
    static RawDict buildRawDictFromTSV(const std::string & path);
    WordsWithTags operator[](std::size_t i) const { return data[i]; }

    std::size_t size() const { return data.size(); }
};
}
