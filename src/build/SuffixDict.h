#ifndef _SUFFIX_DICT_H
#define _SUFFIX_DICT_H
#include "BuildDefs.h"
#include <utils/Misc.h>
#include <memory>
namespace build {

static constexpr std::size_t MAX_TAIL_LENGTH = 5;
class SuffixDict {
private:
	DictPtr sd;
	std::vector<EncodedParadigm> paraMap;
public:
	SuffixDict(const std::vector<EncodedParadigm> &paraMap, DictPtr suffixDict):
		sd(suffixDict), paraMap(paraMap) {}

	ParaPairArray getCandidates(const utils::UniString &word) const;
	friend void dropToFiles(const std::unique_ptr<SuffixDict> &dict, const std::string &dictFilename);
	static std::unique_ptr<SuffixDict> loadSuffixDictFromStream(std::istream & dictIs);
};
}
#endif
