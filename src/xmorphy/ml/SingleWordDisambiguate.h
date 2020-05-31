#pragma once
#include <xmorphy/build/DisambDict.h>
#include <xmorphy/morph/WordForm.h>

namespace X
{
class SingleWordDisambiguate
{
    std::unique_ptr<DisambDict> dict;

public:
    SingleWordDisambiguate(std::istream & is) : dict(DisambDict::loadFromFiles(is)) {}

    SingleWordDisambiguate();
    void disambiguate(std::vector<WordFormPtr> & seq) const;
    void disambiguateSingleForm(WordFormPtr form) const;
};
}
