#pragma once
#include <fstream>
#include <memory>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/utils/Misc.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/build/BuildDefs.h>
namespace X
{

size_t getInnerSpeechPartRepr(UniSPTag sp, UniMorphTag tag);

class PhemDict
{
private:
    PhemDictPtr dict;


public:
    PhemDict(PhemDictPtr dict_)
        : dict(dict_)
    {
    }

    std::vector<PhemTag> getPhemParse(const UniString & word, UniSPTag sp, UniMorphTag tag) const;

    bool contains(const UniString & word) const;

    friend void dropToFiles(PhemDictPtr dict, const std::string & filename);

    static std::unique_ptr<PhemDict> loadFromFiles(std::istream & dict_stream);
};

PhemDictPtr buildPhemDict(std::istream & is);

}
