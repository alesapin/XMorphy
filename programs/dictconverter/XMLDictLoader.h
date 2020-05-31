#pragma once
#include <vector>
#include <xmorphy/tag/MorphTag.h>
#include <xmorphy/tag/SpeechPartTag.h>
#include <xmorphy/utils/UniString.h>

using WordsArray = std::vector<utils::UniString>;
using TagsArray = std::vector<std::tuple<X::SpeechPartTag, X::MorphTag>>;
using RawArray = std::vector<std::pair<WordsArray, TagsArray>>;
using LemataMap = std::vector<std::optional<std::pair<WordsArray, TagsArray>>>;

RawArray buildRawDictFromXML(const std::string & path);
