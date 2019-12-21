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
    for (auto itr = mp.begin(); itr != mp.end(); itr++) {
        if (joined.count(itr->first)) {
            continue;
        }
        WordsArray& parentWords = itr->second.first;
        TagsArray& parentTags = itr->second.second;
        if (linksMap.count(itr->first)) {
            const std::vector<std::size_t>& childs = linksMap.at(itr->first);
            for (std::size_t child : childs) {
                parentWords.insert(parentWords.end(), mp[child].first.begin(),
                                   mp[child].first.end());
                parentTags.insert(parentTags.end(), mp[child].second.begin(),
                                  mp[child].second.end());
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
    std::size_t maxLemmaId = 0;

    std::vector<std::pair<WordsArray, TagsArray>> multiplied;
    for (auto itr : lemmas) {
        maxLemmaId = std::max(itr.first, maxLemmaId);
        WordsArray words;
        TagsArray tags;
        std::tie(words, tags) = itr.second;
        if (std::get<0>(tags[0]) == base::SpeechPartTag::PRTF) {
            base::MorphTag t;
            TagsArray tgs;
            WordsArray wrds;
            for (std::size_t i = 0; i < tags.size(); ++i) {
                std::tie(std::ignore, t) = tags[i];
                if ((t & base::MorphTag::actv) && !(t & base::MorphTag::neut)) {
                    base::MorphTag cs = t.getCase();
                    base::MorphTag gender = t.getGender();
                    base::MorphTag number = t.getNumber();
                    tgs.push_back(
                        std::make_pair(base::SpeechPartTag::NOUN,
                                       cs | gender | number | base::MorphTag::anim));
                    wrds.push_back(words[i]);
                }
            }
            if (!tgs.empty() && wrds.size() == tgs.size()) {
                multiplied.push_back(std::make_pair(wrds, tgs));
            }
        }
    }
    maxLemmaId++;
    std::cerr << "Max Lemma id:" << maxLemmaId << "\n";
    for (size_t i = 0; i < multiplied.size(); ++i) {
        lemmas[i + maxLemmaId] = multiplied[i];
    }
}

}

void buildRawDictFromXML(std::shared_ptr<RawDict>& dict,
                         const std::string& path) {
    using namespace tinyxml2;
    LemataMap lemataMap;
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
            unsigned int lemaId;
            lemma->QueryUnsignedAttribute("id", &lemaId);
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
                tags.push_back(getTags<base::SpeechPartTag, base::MorphTag>(resulttag));
            }
            count++;
            if (count % 1000 == 0) {
                std::cerr << "lemmaid: " << lemaId << "\n";
                std::cerr << "Lemas loaded: " << lemataMap.size() << " lemmas\n";
                std::cerr << "Raw dict loading:" << count << std::endl;
            }
            lemataMap[lemaId] = std::make_pair(words, tags);
        }
    };
    std::cerr << "Totaly loaded: " << lemataMap.size() << " lemmas";
    lemataMultiplier(lemataMap);
    std::cerr << "Totaly after mult:" << lemataMap.size() << " lemmas";
    auto data = joinLemataMap(lemataMap, linksMap);
    dict = std::make_shared<RawDict>();
    dict->data = data;
    dict->filepath = path;
}

}
