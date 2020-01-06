#ifndef _PARADIGM_BUILDER_H
#define _PARADIGM_BUILDER_H
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <tuple>
#include <boost/algorithm/string.hpp>
#include "RawDict.h"
#include "BuildDefs.h"
#include <map>
namespace build {


class ParadigmBuilder {
private:
	std::size_t freqThreshold;
public:
	ParadigmBuilder(std::size_t paradigmFreqThreshold = 1) : freqThreshold(paradigmFreqThreshold) {}
	std::map<Paradigm, std::pair<std::size_t, std::size_t>> getParadigms(const RawDict & rd) const;
};

std::tuple<
	boost::bimap<utils::UniString, std::size_t>,
	boost::bimap<MorphTagPair, std::size_t>,
	boost::bimap<utils::UniString, std::size_t>
	> splitParadigms(const std::map<Paradigm, std::pair<std::size_t,std::size_t>> &paras);

std::map<EncodedParadigm, std::size_t> encodeParadigms(
    const std::map<Paradigm, std::pair<std::size_t, std::size_t>> &paras,
	const boost::bimap<utils::UniString, std::size_t> &prefixes,
	const boost::bimap<MorphTagPair, std::size_t> tags,
	const boost::bimap<utils::UniString, std::size_t> &suffixes);

Paradigm parseOnePara(const WordsArray &words, const TagsArray &tags);
inline std::ostream &operator<<(std::ostream &os, MorphTagPair p) {
	return os << p.sp << " " << p.tag;
}

inline std::istream &operator>>(std::istream &is, MorphTagPair &p) {
	return is >> p.sp >> p.tag;
}

template<typename T>
std::ostream &dropBimapToFile(std::ostream &os, const boost::bimap<T, std::size_t> &m) {
	os << m.size() << "\n";
    for (auto iter = m.begin(), iend = m.end(); iter != iend; ++iter) {
		os << iter->left << "\t" << iter->right << "\n";
	}
	return os;
}

void readBimapFromFile(std::istream &is, boost::bimap<utils::UniString, std::size_t> &m);
void readBimapFromFile(std::istream &is, boost::bimap<MorphTagPair, std::size_t> &m);

}
#endif
