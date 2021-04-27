#pragma once
#include <xmorphy/tag/ITag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/build/BuildDefs.h>
namespace X
{
class DisambDict
{
private:
    DisambDictPtr dict;

public:
    DisambDict(DisambDictPtr dict) : dict(dict) {}
    std::size_t getCount(const UniString & word, UniSPTag sp, UniMorphTag mt) const;
    static constexpr std::size_t MIN_INTERSECTION = 4;
    friend void dropToFiles(const std::unique_ptr<DisambDict> & dict, const std::string & filename);
    static std::unique_ptr<DisambDict> loadFromFiles(std::istream & is);
};
}
