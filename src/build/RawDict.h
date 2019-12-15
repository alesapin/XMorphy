#pragma once
#include <vector>
#include <istream>
#include <map>
#include <set>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include <tinyxml2.h>
#include <utils/UniString.h>
#include <utils/Misc.h>
#include <memory>
#include <tag/MorphTag.h>
#include <tag/SpeechPartTag.h>

namespace build {
using WordsArray = std::vector<utils::UniString>;
using TagsArray = std::vector<std::tuple<base::SpeechPartTag, base::MorphTag>>;
using RawArray = std::vector<std::pair<WordsArray, TagsArray>>;
using LemataMap = std::map<std::size_t, std::pair<WordsArray, TagsArray>>;

template <typename SP, typename MT>
std::tuple<SP, MT> getTags(const std::string& str) {
    std::vector<std::string> tgs;
    boost::split(tgs, str, boost::is_any_of(", |="));

    SP resultSP = SP(tgs[0]);

    tgs.erase(tgs.begin());
    MT resultTag = MT::UNKN;
    for (const auto& tg : tgs) {
        try {
            resultTag |= MT(tg);
        } catch (std::out_of_range e) {
            //std::cerr << "No such tag:" << tg << "\n";
        }
    }
    return std::make_tuple(resultSP, resultTag);
}

class RawDict {
private:
    RawArray data;
    std::string filepath;

public:
    friend void buildRawDictFromXML(std::shared_ptr<RawDict>& dict, const std::string& path);
    std::pair<WordsArray, TagsArray> operator[](std::size_t i) const {
        return data[i];
    }

    std::size_t size() const {
        return data.size();
    }
};
}
