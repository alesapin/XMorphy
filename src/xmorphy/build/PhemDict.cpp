#include <xmorphy/build/PhemDict.h>

namespace X
{

static const auto PARTICIPLE_SP_NUM = UniSPTag::size();
static const auto GERUND_SP_NUM = UniSPTag::size() + 1;
static const auto ADJ_SHORT_SP_NUM = UniSPTag::size() + 2;

size_t getInnerSpeechPartRepr(UniSPTag sp, UniMorphTag tag)
{
    if (sp == UniSPTag::ADJ && tag & UniMorphTag::Short)
    {
        return ADJ_SHORT_SP_NUM;
    }
    else if (sp == UniSPTag::VERB && tag & UniMorphTag::Conv)
    {
        return GERUND_SP_NUM;
    }
    else if (sp == UniSPTag::VERB && tag & UniMorphTag::Part)
    {
        return PARTICIPLE_SP_NUM;
    }

    return X::UniSPTag::get(sp);
}

bool PhemDict::contains(const UniString & word) const
{
    return dict->contains(word.toUpperCase().getRawString());
}

std::vector<PhemTag> PhemDict::getPhemParse(const UniString & word, UniSPTag sp, UniMorphTag tag) const
{
    std::string up_case_word = word.toUpperCase().getRawString();
    PhemMarkup markup = dict->getValue(up_case_word);
    auto it = markup.phem_map.find(getInnerSpeechPartRepr(sp, tag));
    if (it == markup.phem_map.end())
        it = markup.phem_map.begin();

    std::vector<PhemTag> result;
    result.reserve(it->second.size());
    for(size_t i = 0; i < it->second.size(); ++i)
        result.emplace_back(PhemTag::get(it->second[i]));

    return result;
}

void dropToFiles(PhemDictPtr dct, const std::string & filename)
{
    std::ofstream mofs(filename);
    dct->serialize(mofs);
}

std::unique_ptr<PhemDict> PhemDict::loadFromFiles(std::istream & dict_stream)
{
    PhemDictPtr dct = std::make_shared<dawg::Dictionary<PhemMarkup>>();
    dct->deserialize(dict_stream);
    return utils::make_unique<PhemDict>(dct);
}

namespace
{
    std::vector<uint8_t> parsePhemTags(const std::string & phem_parse)
    {
        std::vector<uint8_t> result;
        std::vector<std::string> parts;
        boost::split(parts, phem_parse, boost::is_any_of("/"));
        for (const std::string & part : parts)
        {
            std::vector<std::string> part_and_type;
            boost::split(part_and_type, part, boost::is_any_of(":"));
            UniString text(part_and_type[0]);
            const std::string & type = part_and_type[1];
            size_t start = 0;
            if (type == "SUFF")
            {
                result.push_back(PhemTag::get(PhemTag::B_SUFF));
                start++;
            }
            else if (type == "PREF")
            {
                result.push_back(PhemTag::get(PhemTag::B_PREF));
                start++;
            }
            else if (type == "ROOT")
            {
                result.push_back(PhemTag::get(PhemTag::B_ROOT));
                start++;
            }

            for (size_t i = start; i < text.length(); ++i)
                result.push_back(PhemTag::get(PhemTag(type)));
        }

        return result;
    }

    size_t getSpeechPartRepr(const std::string & morph_info)
    {
        std::vector<PhemTag> result;
        std::vector<std::string> parts;
        boost::split(parts, morph_info, boost::is_any_of(" "));
        std::string speech_part = parts[1];
        speech_part.pop_back();
        if (speech_part == "ADJS")
            return ADJ_SHORT_SP_NUM;
        else if (speech_part == "GRND")
            return GERUND_SP_NUM;
        else if (speech_part == "PART")
            return PARTICIPLE_SP_NUM;
        else if (speech_part == "ADJF")
            return UniSPTag::get(UniSPTag::ADJ);
        else
            return UniSPTag::get(UniSPTag(speech_part));
    }
}

PhemDictPtr buildPhemDict(std::istream & is)
{
    std::string row;
    std::map<std::string, PhemMarkup> counter;
    size_t c = 0;
    while (std::getline(is, row))
    {
        boost::trim(row);
        if (row.empty() || row == "#")
            continue;
        c++;
        if (c % 1000 == 0)
            std::cerr << "Loaded:" << c << std::endl;
        std::vector<std::string> parts;
        boost::split(parts, row, boost::is_any_of("\t"));
        UniString word(parts[0]);
        std::vector<uint8_t> parse = parsePhemTags(parts[1]);
        size_t sp = getSpeechPartRepr(parts.back());
        counter[word.toUpperCase().getRawString()].phem_map[sp] = parse;
    }

    std::cerr << "Counter size:" << counter.size() << std::endl;
    dawg::BuildFactory<PhemMarkup> factory;
    for (auto & [key, value] : counter)
    {
        factory.insertOrLink(key, value);
    }
    return factory.build();
}

}
