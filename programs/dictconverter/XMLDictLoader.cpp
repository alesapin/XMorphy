#include "XMLDictLoader.h"
#include <unordered_map>
#include <tinyxml2.h>

using namespace X;

namespace
{
template <typename SP, typename MT>
std::tuple<SP, MT> getTags(const std::string & str)
{
    std::vector<std::string> tgs;
    boost::split(tgs, str, boost::is_any_of(",|"));

    SP resultSP = SP(tgs[0]);

    tgs.erase(tgs.begin());
    MT resultTag = MT::UNKN;
    for (const auto & tg : tgs)
    {
        try
        {
            if (tg == "Patr" || tg == "Surn" || tg == "Name" || tg == "Abbr" || tg == "Orgn" || tg == "Trad" || tg == "Geox")
            {
                resultTag |= MT::propn;
            }
            else
                resultTag |= MT(tg);
        }
        catch (const std::out_of_range & e)
        {
            //std::cerr << "No such tag:" << tg << "\n";
        }
    }
    return std::make_tuple(resultSP, resultTag);
}

std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, uint8_t>>> getLinks(tinyxml2::XMLElement * links)
{
    using namespace tinyxml2;
    std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::uint8_t>>> result;
    std::unordered_map<std::size_t, std::size_t> toFrom;
    for (XMLElement * link = links->FirstChildElement(); link != nullptr; link = link->NextSiblingElement())
    {
        unsigned int linkFrom, linkTo, linkType;
        link->QueryUnsignedAttribute("type", &linkType);
        link->QueryUnsignedAttribute("from", &linkFrom);
        link->QueryUnsignedAttribute("to", &linkTo);
        if (linkType == 4 || linkType == 7 || linkType == 21 || linkType == 23 || linkType == 27)
            continue;
        if (toFrom.count(linkFrom))
        {
            linkFrom = toFrom[linkFrom];
        }
        result[linkFrom].emplace_back(linkTo, static_cast<uint8_t>(linkType));
        toFrom[linkTo] = linkFrom;
    }
    return result;
}
RawArray joinLemataMap(LemataMap & mp, const std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, uint8_t>>> & linksMap)
{
    std::cerr << "Joining lemata\n";
    std::set<std::size_t> joined;
    for (auto itr : linksMap)
    {
        for (auto [child, linkType] : itr.second)
            joined.insert(child);
    }
    std::cerr << "Joined size:" << joined.size() << "\n";
    RawArray result;

    std::size_t count = 0;
    for (size_t lemmaId = 0; lemmaId < mp.size(); ++lemmaId)
    {
        if (!mp[lemmaId] || joined.count(lemmaId))
        {
            continue;
        }
        WordsArray & parentWords = mp[lemmaId]->words_array;
        TagsArray & parentTags = mp[lemmaId]->tags_array;
        std::vector<bool> & parentNf = mp[lemmaId]->nf_mask;
        if (linksMap.count(lemmaId))
        {
            const std::vector<std::pair<std::size_t, uint8_t>> & childs = linksMap.at(lemmaId);
            for (auto [child, linkType] : childs)
            {
                std::cerr << "Child:" << child << " linkid:" << (int)(linkType) << std::endl;
                if (!mp[child])
                    continue;

                WordsArray & childWords = mp[child]->words_array;
                TagsArray & childTags = mp[child]->tags_array;
                std::vector<bool> & childNf = mp[child]->nf_mask;

                parentWords.insert(parentWords.end(), childWords.begin(), childWords.end());
                parentTags.insert(parentTags.end(), childTags.begin(), childTags.end());
                auto [sp, tag] = childTags[0];
                if (sp == SpeechPartTag::PRTF || sp == SpeechPartTag::PRTS)
                    parentNf.insert(parentNf.end(), childNf.begin(), childNf.end());
                else
                    parentNf.resize(parentNf.size() + childNf.size(), false);
            }
        }
        count++;
        if (count % 1000 == 0)
        {
            std::cerr << "Lemas joining:" << count << std::endl;
        }
        result.push_back({parentWords, parentTags, parentNf});
    }
    std::cerr << "Join finished\n";
    mp.clear();
    return result;
}

void lemataMultiplier(LemataMap & lemmas)
{
    size_t maxLemmaId = 0;

    std::vector<LemmataRecord> multiplied;
    for (size_t lemmaId = 0; lemmaId < lemmas.size(); ++lemmaId)
    {
        if (!lemmas[lemmaId])
            continue;
        maxLemmaId = std::max(lemmaId, maxLemmaId);
        auto [words, tags, nf_mask] = *lemmas[lemmaId];
        if (std::get<0>(tags[0]) == SpeechPartTag::PRTF)
        {
            MorphTag t;
            TagsArray tgs;
            WordsArray wrds;
            std::vector<bool> nf_mask;
            bool first = true;
            for (std::size_t i = 0; i < tags.size(); ++i)
            {
                std::tie(std::ignore, t) = tags[i];
                if ((t & MorphTag::actv) && !(t & MorphTag::neut))
                {
                    MorphTag cs = t.getCase();
                    MorphTag gender = t.getGender();
                    MorphTag number = t.getNumber();

                    tgs.push_back(std::make_pair(SpeechPartTag::NOUN, cs | gender | number | MorphTag::anim | MorphTag::actv));
                    wrds.push_back(words[i]);
                    nf_mask.push_back(first);
                    first = false;
                }
            }
            if (!tgs.empty() && wrds.size() == tgs.size())
            {
                multiplied.push_back({wrds, tgs, nf_mask});
            }
        }
    }
    if (lemmas.size() <= multiplied.size() + maxLemmaId)
        lemmas.resize(multiplied.size() + maxLemmaId + 1);

    maxLemmaId++;
    std::cerr << "Max Lemma id:" << maxLemmaId << "\n";
    for (size_t i = 0; i < multiplied.size(); ++i)
    {
        lemmas[i + maxLemmaId] = multiplied[i];
    }
}

}

RawArray buildRawDictFromXML(const std::string & path)
{
    using namespace tinyxml2;
    LemataMap lemataMap;
    lemataMap.resize(400000); // Some big number
    std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, uint8_t>>> linksMap;
    {
        XMLDocument mainDoc;
        mainDoc.LoadFile(path.c_str());
        XMLElement * dictionary = mainDoc.FirstChildElement("dictionary");
        linksMap = getLinks(dictionary->FirstChildElement("links"));
        XMLElement * lemmata = dictionary->FirstChildElement("lemmata");
        std::size_t count = 0;
        for (XMLElement * lemma = lemmata->FirstChildElement(); lemma != nullptr; lemma = lemma->NextSiblingElement())
        {
            XMLElement * nf = lemma->FirstChildElement("l");
            unsigned int lemmaId;
            lemma->QueryUnsignedAttribute("id", &lemmaId);
            WordsArray words;
            TagsArray tags;
            std::vector<bool> nf_mask;
            std::string overalltag;
            if (!nf->NoChildren())
            {
                for (XMLElement * tag = nf->FirstChildElement(); tag != nullptr; tag = tag->NextSiblingElement())
                {
                    const char * tagText = tag->Attribute("v");
                    overalltag += std::string(tagText) + ",";
                }
            }
            bool first = true;
            for (XMLElement * form = lemma->FirstChildElement("f"); form != nullptr; form = form->NextSiblingElement())
            {
                UniString formText(form->Attribute("t"));
                std::string currenttag;
                if (!nf->NoChildren())
                {
                    for (XMLElement * tag = form->FirstChildElement(); tag != nullptr; tag = tag->NextSiblingElement())
                    {
                        const char * tagText = tag->Attribute("v");
                        currenttag += std::string(tagText) + ",";
                    }
                }
                std::string resulttag = overalltag + currenttag;
                if (!resulttag.empty())
                {
                    resulttag.pop_back();
                }
                words.push_back(formText.toUpperCase());
                tags.push_back(getTags<SpeechPartTag, MorphTag>(resulttag));
                nf_mask.push_back(first);
                first = false;
            }
            count++;
            if (count % 1000 == 0)
            {
                std::cerr << "lemmaid: " << lemmaId << "\n";
                std::cerr << "Raw dict loading:" << count << std::endl;
            }
            if (lemataMap.size() <= lemmaId)
            {
                lemataMap.resize(lemmaId + 1);
            }
            lemataMap[lemmaId] = {words, tags, nf_mask};
        }
    };
    std::cerr << "Totaly loaded: " << lemataMap.size() << " lemmas";
    lemataMultiplier(lemataMap);
    std::cerr << "Totaly after mult:" << lemataMap.size() << " lemmas";
    return joinLemataMap(lemataMap, linksMap);
}
