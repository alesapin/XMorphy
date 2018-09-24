#ifndef _DISAMB_DICT
#define _DISAMB_DICT
#include "BuildDefs.h"
#include <utils/UniString.h>
#include <tag/MorphTag.h>
#include <tag/SpeechPartTag.h>
#include <tag/ITag.h>
#include <utils/Misc.h>
#include <tag/SpeechPartTag.h>
#include <tag/MorphTag.h>
#include <boost/algorithm/string.hpp>
namespace build {
class DisambDict {
private:
	DisambDictPtr dict;
public:
	DisambDict(DisambDictPtr dict): dict(dict) {}
	std::size_t getCount(const utils::UniString &word, base::SpeechPartTag sp, base::MorphTag mt) const;
	static constexpr std::size_t MIN_INTERSECTION = 4;
	friend void dropToFiles(const std::unique_ptr<DisambDict> &dict, const std::string &filename);
	friend void loadFromFiles(std::unique_ptr<DisambDict> &dict, const std::string &filename);
};
}
#endif
