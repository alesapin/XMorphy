#pragma once
#include <boost/algorithm/string.hpp>
#include <tag/ITag.h>
#include <tag/UniMorphTag.h>
#include <tag/UniSPTag.h>
#include <utils/Misc.h>
#include <utils/UniString.h>
#include "BuildDefs.h"
namespace X
{
class DisambDict
{
private:
    DisambDictPtr dict;

public:
    DisambDict(DisambDictPtr dict) : dict(dict) {}
    std::size_t getCount(const utils::UniString & word, UniSPTag sp, UniMorphTag mt) const;
    static constexpr std::size_t MIN_INTERSECTION = 4;
    friend void dropToFiles(const std::unique_ptr<DisambDict> & dict, const std::string & filename);
    static std::unique_ptr<DisambDict> loadFromFiles(std::istream & is);
};
}
