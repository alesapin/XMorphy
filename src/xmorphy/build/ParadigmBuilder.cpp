#include <xmorphy/build/ParadigmBuilder.h>
#include <exception>
namespace X
{
Paradigm parseOnePara(const WordsArray & words, const TagsArray & tags, const std::vector<bool> & nf_mask)
{
    UniString commonPref = longestCommonPrefix(words);
    UniString commonPart = longestCommonSubstring(words);
    UniString common;
    if (commonPref.length() > commonPart.length())
    {
        common = commonPref;
    }
    else
    {
        common = commonPart;
    }
    Paradigm result;
    for (std::size_t i = 0; i < words.size(); ++i)
    {
        long pos = words[i].find(common);
        UniString prefix = words[i].subString(0, pos);
        UniString suffix = words[i].subString(pos + common.length());
        auto [resultSP, resultTag] = tags[i];
        if (resultSP == UniSPTag::X)
            throw std::runtime_error("Found empty speech part for '" + words[i].getRawString() + "'");
        result.emplace_back(LexemeGroup{prefix, resultSP, resultTag, suffix, nf_mask[i]});
    }
    return result;
}

std::map<Paradigm, ParadigmOccurences> ParadigmBuilder::getParadigms(const RawDict & rd) const
{
    std::string row;
    std::map<Paradigm, ParadigmOccurences> result;
    std::cerr << "Getting paradimgs\n";
    std::map<Paradigm, std::size_t> counter;
    for (std::size_t i = 0; i < rd.size(); ++i)
    {
        try
        {
            counter[parseOnePara(rd[i].words, rd[i].tags, rd[i].nf_mask)]++;
        }
        catch (const std::exception & e)
        {
            std::cerr << "Caught exception on word:" << rd[i].words[0] << " tags: " << rd[i].tags[0].tag;
        }
    }
    std::cerr << "Paradimgs getted\n";
    std::size_t number = 0;
    for (auto itr : counter)
    {
        if (itr.second >= freqThreshold)
        {
            result[itr.first] = ParadigmOccurences{number++, itr.second};
        }
    }
    std::cerr << "Paradimgs counted\n";
    return result;
}

IntermediateParadigmsState splitParadigms(const std::map<Paradigm, ParadigmOccurences> & paras)
{
    StringToIndexBiMap prefixes, suffixes;
    TagToIndexBiMap tags;
    std::cerr << "Start spliting\n";
    for (auto itr : paras)
    {
        for (const LexemeGroup & lg : itr.first)
        {
            MorphTagPair tp{lg.sp, lg.tag};
            if (prefixes.left.find(lg.prefix) == prefixes.left.end())
            {
                prefixes.insert({lg.prefix, prefixes.size()});
            }
            if (suffixes.left.find(lg.suffix) == suffixes.left.end())
            {
                suffixes.insert({lg.suffix, suffixes.size()});
            }
            if (tags.left.find(tp) == tags.left.end())
            {
                tags.insert({tp, tags.size()});
            }
        }
    }
    std::cerr << "Spliting finished:" << prefixes.size() << " tags size:" << tags.size() << " suffs:" << suffixes.size() << "\n";
    return IntermediateParadigmsState{std::move(prefixes), std::move(tags), std::move(suffixes)};
}

std::map<EncodedParadigm, std::size_t>
encodeParadigms(const std::map<Paradigm, ParadigmOccurences> & paras, const IntermediateParadigmsState & intermediateState)
{
    std::map<EncodedParadigm, std::size_t> result;
    std::cerr << "Encoding paradigms\n";
    for (const auto & [paradigm, _] : paras)
    {
        EncodedParadigm epar(paradigm.size());
        for (std::size_t i = 0; i < paradigm.size(); ++i)
        {
            std::size_t prefixId = intermediateState.prefixesMap.left.at(paradigm[i].prefix);
            std::size_t tagId = intermediateState.tagsMap.left.at(MorphTagPair{paradigm[i].sp, paradigm[i].tag});
            std::size_t suffixId = intermediateState.suffixesMap.left.at(paradigm[i].suffix);
            epar[i] = EncodedLexemeGroup{prefixId, tagId, suffixId, paradigm[i].isNormalForm};
        }
        if (!result.count(epar))
        {
            result[epar] = result.size();
        }
    }
    std::cerr << "Encoding finished\n";
    return result;
}

void readBimapFromFile(std::istream & is, boost::bimap<UniString, std::size_t> & m)
{
    std::size_t size, counter = 0;
    std::string row;
    std::getline(is, row);
    size = stoul(row);
    while (counter < size)
    {
        std::getline(is, row);
        std::vector<std::string> splited;
        boost::split(splited, row, boost::is_any_of("\t"));
        m.insert({UniString(splited[0]), std::stoul(splited[1])});
        counter++;
    }
}

void readMapFromFile(std::istream & is, std::unordered_map<size_t, UniString> & m)
{
    std::size_t size, counter = 0;
    std::string row;
    std::getline(is, row);
    size = stoul(row);
    while (counter < size)
    {
        std::getline(is, row);
        std::vector<std::string> splited;
        boost::split(splited, row, boost::is_any_of("\t"));
        m[std::stoul(splited[1])] = UniString(splited[0]);
        counter++;
    }
}

void readBimapFromFile(std::istream & is, boost::bimap<MorphTagPair, std::size_t> & m)
{
    std::size_t size, counter = 0;
    std::string row;
    std::getline(is, row);
    size = stoul(row);
    while (counter < size)
    {
        MorphTagPair p;
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
