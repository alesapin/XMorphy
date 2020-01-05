#include "RawDict.h"
#include <fstream>
namespace build {

namespace {

std::unordered_map<std::size_t, std::vector<std::size_t>>
getLinks(tinyxml2::XMLElement* links) {
    using namespace tinyxml2;
    std::unordered_map<std::size_t, std::vector<std::size_t>> result;
    std::unordered_map<std::size_t, std::size_t> toFrom;
    for (XMLElement* link = links->FirstChildElement(); link != nullptr;
         link = link->NextSiblingElement()) {
        unsigned int linkFrom, linkTo, linkType;
        link->QueryUnsignedAttribute("type", &linkType);
        link->QueryUnsignedAttribute("from", &linkFrom);
        link->QueryUnsignedAttribute("to", &linkTo);
        if (linkType == 4 || linkType == 7 || linkType == 21 || linkType == 23 || linkType == 27) continue;
        if (toFrom.count(linkFrom)) {
            linkFrom = toFrom[linkFrom];
        }
        result[linkFrom].push_back(linkTo);
        toFrom[linkTo] = linkFrom;
    }
    return result;
}

RawArray joinLemataMap(
    LemataMap& mp,
    const std::unordered_map<std::size_t, std::vector<std::size_t>>& linksMap) {
    std::cerr << "Joining lemata\n";
    std::set<std::size_t> joined;
    for (auto itr : linksMap) {
        joined.insert(itr.second.begin(), itr.second.end());
    }
    std::cerr << "Joined size:" << joined.size() << "\n";
    RawArray result;

    std::size_t count = 0;
    for (size_t lemmaId = 0; lemmaId < mp.size(); ++lemmaId) {
        if (!mp[lemmaId] || joined.count(lemmaId)) {
            continue;
        }
        WordsArray& parentWords = mp[lemmaId]->first;
        TagsArray& parentTags = mp[lemmaId]->second;
        if (linksMap.count(lemmaId)) {
            const std::vector<std::size_t>& childs = linksMap.at(lemmaId);
            for (std::size_t child : childs) {
                if (!mp[child])
                    continue;
                parentWords.insert(parentWords.end(), mp[child]->first.begin(),
                                   mp[child]->first.end());
                parentTags.insert(parentTags.end(), mp[child]->second.begin(),
                                  mp[child]->second.end());
            }
        }
        count++;
        if (count % 1000 == 0) {
            std::cerr << "Lemas joining:" << count << std::endl;
        }
        result.push_back(std::make_pair(parentWords, parentTags));
    }
    std::cerr << "Join finished\n";
    mp.clear();
    return result;
}

void lemataMultiplier(LemataMap& lemmas) {
    size_t maxLemmaId = 0;

    std::vector<std::pair<WordsArray, TagsArray>> multiplied;
    for (size_t lemmaId = 0; lemmaId < lemmas.size(); ++lemmaId) {
        if (!lemmas[lemmaId])
            continue;
        maxLemmaId = std::max(lemmaId, maxLemmaId);
        auto [words, tags] = *lemmas[lemmaId];
        if (std::get<0>(tags[0]) == base::UniSPTag::ADJ) {
            base::UniMorphTag t;
            TagsArray tgs;
            WordsArray wrds;
            for (std::size_t i = 0; i < tags.size(); ++i) {
                std::tie(std::ignore, t) = tags[i];
                if ((t & base::UniMorphTag::Act) && !(t & base::UniMorphTag::Neut)) {
                    base::UniMorphTag cs = t.getCase();
                    base::UniMorphTag gender = t.getGender();
                    base::UniMorphTag number = t.getNumber();
                    tgs.push_back(
                        std::make_pair(base::UniSPTag::NOUN,
                            cs | gender | number | base::UniMorphTag::Anim));
                    wrds.push_back(words[i]);
                }
            }
            if (!tgs.empty() && wrds.size() == tgs.size()) {
                multiplied.push_back(std::make_pair(wrds, tgs));
            }
        }
    }
    if (lemmas.size() <= multiplied.size() + maxLemmaId)
        lemmas.resize(multiplied.size() + maxLemmaId + 1);

    maxLemmaId++;
    std::cerr << "Max Lemma id:" << maxLemmaId << "\n";
    for (size_t i = 0; i < multiplied.size(); ++i) {
        lemmas[i + maxLemmaId] = multiplied[i];
    }
}

}

RawDict RawDict::buildRawDictFromXML(const std::string& path)
{
    using namespace tinyxml2;
    LemataMap lemataMap;
    lemataMap.resize(400000); // Some big number
    std::unordered_map<std::size_t, std::vector<std::size_t>> linksMap;
    {
        XMLDocument mainDoc;
        mainDoc.LoadFile(path.c_str());
        XMLElement* dictionary = mainDoc.FirstChildElement("dictionary");
        linksMap = getLinks(dictionary->FirstChildElement("links"));
        XMLElement* lemmata = dictionary->FirstChildElement("lemmata");
        std::size_t count = 0;
        for (XMLElement* lemma = lemmata->FirstChildElement(); lemma != nullptr;
             lemma = lemma->NextSiblingElement()) {
            XMLElement* nf = lemma->FirstChildElement("l");
            unsigned int lemmaId;
            lemma->QueryUnsignedAttribute("id", &lemmaId);
            WordsArray words;
            TagsArray tags;
            std::string overalltag;
            if (!nf->NoChildren()) {
                for (XMLElement* tag = nf->FirstChildElement(); tag != nullptr;
                     tag = tag->NextSiblingElement()) {
                    const char* tagText = tag->Attribute("v");
                    overalltag += std::string(tagText) + ",";
                }
            }
            for (XMLElement* form = lemma->FirstChildElement("f"); form != nullptr;
                 form = form->NextSiblingElement()) {
                utils::UniString formText(form->Attribute("t"));
                std::string currenttag;
                if (!nf->NoChildren()) {
                    for (XMLElement* tag = form->FirstChildElement(); tag != nullptr;
                         tag = tag->NextSiblingElement()) {
                        const char* tagText = tag->Attribute("v");
                        currenttag += std::string(tagText) + ",";
                    }
                }
                std::string resulttag = overalltag + currenttag;
                if (!resulttag.empty()) {
                    resulttag.pop_back();
                }
                words.push_back(formText.toUpperCase().replace(
                    utils::UniCharacter::YO, utils::UniCharacter::YE));
                tags.push_back(
                    getTags<base::UniSPTag, base::UniMorphTag>(resulttag));
            }
            count++;
            if (count % 1000 == 0) {
                std::cerr << "lemmaid: " << lemmaId << "\n";
                std::cerr << "Raw dict loading:" << count << std::endl;
            }
            if (lemataMap.size() <= lemmaId)
            {
                lemataMap.resize(lemmaId + 1);
            }
            lemataMap[lemmaId] = std::make_pair(words, tags);
        }
    };
    std::cerr << "Totaly loaded: " << lemataMap.size() << " lemmas";
    lemataMultiplier(lemataMap);
    std::cerr << "Totaly after mult:" << lemataMap.size() << " lemmas";
    auto data = joinLemataMap(lemataMap, linksMap);
    return RawDict(std::move(data), path);
}

RawDict RawDict::buildRawDictFromTSV(const std::string& path)
{
    std::ifstream ifs(path);

    size_t counter = 0;
    RawArray resultArray;
    while (!ifs.eof()) {
        WordsArray words;
        TagsArray tags;
        std::string current;
        std::getline(ifs, current);
        while (!current.empty()) {
            std::vector<std::string> parts;
            boost::split(parts, current, boost::is_any_of("\t"));
            if (parts.size() != 4)
                throw std::runtime_error("Error parsing string '" + current + "'");
            words.emplace_back(std::move(parts[0]));
            tags.emplace_back(getTags<base::UniSPTag, base::UniMorphTag>(parts[2] + "|" + parts[3]));
            std::getline(ifs, current);
            counter++;
            if (counter % 1000 == 0)
                std::cerr << "Processed: " << counter  << " words"<< std::endl;
        }
        if (words.empty())
            continue;
        resultArray.emplace_back(std::make_pair(std::move(words), std::move(tags)));
    }
    return RawDict(std::move(resultArray), path);
}

}
