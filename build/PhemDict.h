#ifndef _PHEM_DICT_H
#define _PHEM_DICT_H
#include "../tag/PhemTag.h"
#include "BuildDefs.h"
#include "../utils/UniString.h"
#include "../utils/Misc.h"
#include <memory>
#include <fstream>
namespace build {
class PhemDict {
private:
    InnerPhemDictPtr dict;
    InnerCounterPhemDictPtr fdict;
    InnerCounterPhemDictPtr bdict;

    std::size_t countAffix(const utils::UniString& affix, InnerCounterPhemDictPtr dptr) const {
        std::size_t prefs = 0;
        std::string raw = affix.toUpperCase().getRawString();
        if (dptr->contains(raw)) {
            prefs = dptr->getValue(raw);
        }
        return prefs;
    }

public:
    PhemDict(InnerPhemDictPtr dict, InnerCounterPhemDictPtr fdict, InnerCounterPhemDictPtr bdict)
        : dict(dict)
        , fdict(fdict)
        , bdict(bdict) {
    }

    std::vector<base::PhemTag> getPhemParse(const utils::UniString& word) const;
    bool contains(const utils::UniString& word) const;
    std::size_t countPrefix(const utils::UniString& prefix) const {
        return countAffix(prefix, fdict);
    }
    std::size_t countSuffix(const utils::UniString& suffix) const {
        return countAffix(suffix.reverse(), bdict);
    }

    static const std::string MAIN_PHEM;
    static const std::string FORWARD_PHEM;
    static const std::string BACKWARD_PHEM;

    friend void dropToFiles(const std::unique_ptr<PhemDict>& dict, const std::string& filename);
    friend void loadFromFiles(std::unique_ptr<PhemDict>& dict, const std::string& filename);
};
}
#endif
