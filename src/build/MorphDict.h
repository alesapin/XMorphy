#ifndef _MORPH_DICT_H
#define _MORPH_DICT_H
#include <utils/UniString.h>
#include <boost/bimap.hpp>
#include <boost/algorithm/string.hpp>
#include <memory>
#include "ParadigmBuilder.h"
#include <DAWG/Dictionary.h>
#include <DAWG/BuildFactory.h>
#include <utils/Misc.h>
#include "BuildDefs.h"
namespace build {

struct MorphDictInfo
{
    LexemeGroup lexemeGroup;
    AffixPair affixPair;
    size_t occurences;
};

class MorphDict {

public:
	MorphDict(const std::vector<EncodedParadigm> &paraMap,
			  DictPtr mainDict,
			  const StringToIndexBiMap &prefs,
			  const TagToIndexBiMap &tags,
			  const StringToIndexBiMap &sufs) :
		paraMap(paraMap),
		mainDict(mainDict),
		prefixes(prefs),
		tags(tags),
		suffixes(sufs)
	{
    }

	std::vector<LexemeGroup> getForms(const utils::UniString &form) const;
	std::vector<MorphDictInfo> getClearForms(const utils::UniString &form) const;
	std::map<Paradigm, std::size_t> getParadigmsForForm(const utils::UniString &form) const;
	void getClearForms(const ParaPairArray &arr, std::vector<MorphDictInfo> &result) const;
	void getParadigmsForForm(const ParaPairArray &arr, std::map<Paradigm, std::size_t> &result) const;

	bool contains(const utils::UniString &form) const {
		return mainDict->contains(form.getRawString());
	}
	friend void dropToFiles(const std::unique_ptr<MorphDict> &dict, const std::string &mainDictFilename, const std::string &affixesFileName);
	static std::unique_ptr<MorphDict> loadFromFiles(std::istream & mainDictIs, std::istream & affixesIs);

private:
	std::vector<EncodedParadigm> paraMap;
	DictPtr mainDict;
	StringToIndexBiMap prefixes;
	TagToIndexBiMap tags;
	StringToIndexBiMap suffixes;

	Paradigm decodeParadigm(const EncodedParadigm& para) const;

};

}
#endif
