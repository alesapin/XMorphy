#ifndef _SINGLE_WORD_DISAMBIGUATE_H
#define _SINGLE_WORD_DISAMBIGUATE_H
#include "IDisambig.h"
#include <build/DisambDict.h>
#include <Resource.h>
namespace disamb {
class SingleWordDisambiguate : public IDisambig {
	std::unique_ptr<build::DisambDict> dict;
public:
	SingleWordDisambiguate(std::istream & is)
        : dict(build::DisambDict::loadFromFiles(is))
    {}

    SingleWordDisambiguate() {
        const auto& factory = CppResource::ResourceFactory::instance();
        std::istringstream disambdict(factory.getAsString("disambdict"));
        dict = build::DisambDict::loadFromFiles(disambdict);
    }

    void disambiguate(std::vector<analyze::WordFormPtr> &seq) const override;
};
}
#endif
