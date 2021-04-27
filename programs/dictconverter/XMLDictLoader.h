#pragma once
#include <vector>
#include <xmorphy/tag/MorphTag.h>
#include <xmorphy/tag/SpeechPartTag.h>
#include <xmorphy/utils/UniString.h>

using WordsArray = std::vector<X::UniString>;
using TagsArray = std::vector<std::tuple<X::SpeechPartTag, X::MorphTag>>;
struct LemmataRecord
{
    WordsArray words_array;
    TagsArray tags_array;
    std::vector<bool> nf_mask;
};

using RawArray = std::vector<LemmataRecord>;
using LemataMap = std::vector<std::optional<LemmataRecord>>;

RawArray buildRawDictFromXML(const std::string & path);
