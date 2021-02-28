#include <xmorphy/build/RawDict.h>

#include <iostream>
#include <fstream>

namespace X
{
RawDict RawDict::buildRawDictFromTSV(const std::string & path)
{
    std::ifstream ifs(path);

    size_t counter = 0;
    RawArray resultArray;
    while (!ifs.eof())
    {
        WordsArray words;
        TagsArray tags;
        std::vector<bool> nf_mask;
        std::string current;
        std::getline(ifs, current);
        while (!current.empty())
        {
            std::vector<std::string> parts;
            boost::split(parts, current, boost::is_any_of("\t"));
            if (parts.size() != 5)
                throw std::runtime_error("Error parsing string '" + current + "'");
            words.emplace_back(std::move(parts[0]));
            auto [spTag, morphTag] = getTags<UniSPTag, UniMorphTag>(parts[2] + "|" + parts[3]);
            if (spTag == UniSPTag::X)
                throw std::runtime_error(
                    "Speech part tag empty for '" + words.back().getRawString() + "', with tag part '" + parts[2] + "|" + parts[3] + "'");
            tags.emplace_back(MorphTagPair{spTag, morphTag});
            std::getline(ifs, current);
            counter++;
            nf_mask.push_back(parts[4] == "1");
            if (counter % 10000 == 0)
                std::cerr << "Processed:" << counter << std::endl;
        }
        if (words.empty())
            continue;
        resultArray.emplace_back(WordsWithTags{std::move(words), std::move(tags), std::move(nf_mask)});
    }
    return RawDict(std::move(resultArray), path);
}

}
