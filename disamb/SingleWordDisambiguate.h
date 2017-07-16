#ifndef _SINGLE_WORD_DISAMBIGUATE_H
#define _SINGLE_WORD_DISAMBIGUATE_H
#include "IDisambig.h"
#include "../build/DisambDict.h"
namespace disamb {
class SingleWordDisambiguate : public IDisambig {
	std::unique_ptr<build::DisambDict> dict;
public:
	SingleWordDisambiguate(const std::string &dictPath) {
		loadFromFiles(dict, dictPath);
	}
	void disambiguate(std::vector<analyze::WordFormPtr> &seq) const override;
};
}
#endif
