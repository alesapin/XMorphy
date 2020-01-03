#include "ParadigmBuilder.h"
namespace build {
Paradigm parseOnePara(const WordsArray& words, const TagsArray& tags) {
    utils::UniString commonPref = longestCommonPrefix(words);
    utils::UniString commonPart = longestCommonSubstring(words);
    utils::UniString common;
    if (commonPref.length() > commonPart.length()) {
        common = commonPref;
    } else {
        common = commonPart;
    }
    Paradigm result;
    for (std::size_t i = 0; i < words.size(); ++i) {
        long pos = words[i].find(common);
        utils::UniString prefix = words[i].subString(0, pos);
        utils::UniString suffix = words[i].subString(pos + common.length());
        base::SpeechPartTag resultSP = base::SpeechPartTag::UNKN;
        base::MorphTag resultTag = base::MorphTag::UNKN;
        std::tie(resultSP, resultTag) = tags[i];
        result.push_back(std::make_tuple(prefix, resultSP, resultTag, suffix));
    }
    return result;
}

std::map<Paradigm, std::pair<std::size_t, std::size_t>>
OpenCorporaParadigmBuilder::getParadigms(const RawDict & rd) const {
    std::string row;
    std::map<Paradigm, std::pair<std::size_t, std::size_t>> result;
    std::cerr << "Getting paradimgs\n";
    std::map<Paradigm, std::size_t> counter;
    for (std::size_t i = 0; i < rd.size(); ++i) {
        WordsArray words;
        TagsArray tags;
        std::tie(words, tags) = rd[i];
        try {
            counter[parseOnePara(words, tags)]++;
        } catch (const std::exception & e) {
            std::cerr << "Caught exception on word:" << words[0] << " tags: " << std::get<1>(tags[0]);
        }
    }
    std::cerr << "Paradimgs getted\n";
    std::size_t number = 0;
    for (auto itr : counter) {
        if (itr.second >= freqThreshold) {
            result[itr.first] = std::make_pair(number++, itr.second);
        }
    }
    std::cerr << "Paradimgs counted\n";
    return result;
}

std::tuple<
    boost::bimap<utils::UniString, std::size_t>,
    boost::bimap<TagPair, std::size_t>,
    boost::bimap<utils::UniString, std::size_t>>
splitParadigms(const std::map<Paradigm, std::pair<std::size_t, std::size_t>>& paras) {
    boost::bimap<utils::UniString, std::size_t> prefixes, suffixes;
    boost::bimap<TagPair, std::size_t> tags;
    std::cerr << "Start spliting\n";
    for (auto itr : paras) {
        for (const LexemeGroup& lg : itr.first) {
            utils::UniString prefix, suffix;
            base::SpeechPartTag sp = base::SpeechPartTag::UNKN;
            base::MorphTag mt = base::MorphTag::UNKN;

            std::tie(prefix, sp, mt, suffix) = lg;
            TagPair tp = std::make_pair(sp, mt);
            if (prefixes.left.find(prefix) == prefixes.left.end()) {
                prefixes.insert({prefix, prefixes.size()});
            }
            if (suffixes.left.find(suffix) == suffixes.left.end()) {
                suffixes.insert({suffix, suffixes.size()});
            }
            if (tags.left.find(tp) == tags.left.end()) {
                tags.insert({tp, tags.size()});
            }
        }
    }
    std::cerr << "Spliting finished:" << prefixes.size() << " tags size:" << tags.size() << " suffs:" << suffixes.size() << "\n";
    return std::make_tuple(prefixes, tags, suffixes);
}

std::map<EncodedParadigm, std::size_t> encodeParadigms(
    const std::map<Paradigm, std::pair<std::size_t, std::size_t>>& paras,
    const boost::bimap<utils::UniString, std::size_t>& prefixes,
    const boost::bimap<TagPair, std::size_t> tags,
    const boost::bimap<utils::UniString, std::size_t>& suffixes)
{
    std::map<EncodedParadigm, std::size_t> result;
    std::cerr << "Encoding paradigms\n";
    for (auto itr : paras) {
        EncodedParadigm epar(itr.first.size());
        for (std::size_t i = 0; i < itr.first.size(); ++i) {
            std::size_t prefixId = prefixes.left.at(std::get<0>(itr.first[i]));
            std::size_t tagId = tags.left.at(std::make_pair(std::get<1>(itr.first[i]), std::get<2>(itr.first[i])));
            std::size_t suffixId = suffixes.left.at(std::get<3>(itr.first[i]));
            std::tuple<std::size_t, std::size_t, std::size_t> value(prefixId, tagId, suffixId);
            epar[i] = value;
        }
        if (!result.count(epar)) {
            result[epar] = result.size();
        }
    }
    std::cerr << "Encoding finished\n";
    return result;
}

void readBimapFromFile(std::istream& is, boost::bimap<utils::UniString, std::size_t>& m) {
    std::size_t size, counter = 0;
    std::string row;
    std::getline(is, row);
    size = stoul(row);
    while (counter < size) {
        std::getline(is, row);
        std::vector<std::string> splited;
        boost::split(splited, row, boost::is_any_of("\t"));
        m.insert({utils::UniString(splited[0]), std::stoul(splited[1])});
        counter++;
    }
}
void readBimapFromFile(std::istream& is, boost::bimap<TagPair, std::size_t>& m) {
    std::size_t size, counter = 0;
    std::string row;
    std::getline(is, row);
    size = stoul(row);
    while (counter < size) {
        TagPair p;
        std::size_t c;
        std::getline(is, row);
        std::vector<std::string> splited;
        boost::split(splited, row, boost::is_any_of("\t"));
        std::istringstream iss(splited[0]);
        iss >> p;
        c = stoul(splited[1]);
        m.insert({p, c});
        counter++;
    }
}
}
