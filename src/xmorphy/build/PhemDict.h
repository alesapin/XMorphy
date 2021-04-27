#pragma once
#include <fstream>
#include <memory>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/build/BuildDefs.h>
namespace X
{
class PhemDict
{
private:
    InnerPhemDictPtr dict;
    InnerCounterPhemDictPtr fdict;
    InnerCounterPhemDictPtr bdict;

    std::size_t countAffix(const UniString & affix, InnerCounterPhemDictPtr dptr) const
    {
        std::size_t prefs = 0;
        std::string raw = affix.toUpperCase().getRawString();
        if (dptr->contains(raw))
        {
            prefs = dptr->getValue(raw);
        }
        return prefs;
    }

public:
    PhemDict(InnerPhemDictPtr dict, InnerCounterPhemDictPtr fdict, InnerCounterPhemDictPtr bdict) : dict(dict), fdict(fdict), bdict(bdict)
    {
    }

    std::vector<PhemTag> getPhemParse(const UniString & word) const;
    bool contains(const UniString & word) const;
    std::size_t countPrefix(const UniString & prefix) const { return countAffix(prefix, fdict); }
    std::size_t countSuffix(const UniString & suffix) const { return countAffix(suffix.reverse(), bdict); }

    static const std::string MAIN_PHEM;
    static const std::string FORWARD_PHEM;
    static const std::string BACKWARD_PHEM;

    friend void dropToFiles(const std::unique_ptr<PhemDict> & dict, const std::string & filename);

    static std::unique_ptr<PhemDict> loadFromFiles(std::istream & mainDictIs, std::istream & forwardIs, std::istream & backwardIs);
};
}
