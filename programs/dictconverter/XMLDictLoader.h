#pragma once
#include <vector>
#include <tag/MorphTag.h>
#include <tag/SpeechPartTag.h>
#include <utils/UniString.h>

using WordsArray = std::vector<utils::UniString>;
using TagsArray = std::vector<std::tuple<base::SpeechPartTag, base::MorphTag>>;
using RawArray = std::vector<std::pair<WordsArray, TagsArray>>;
using LemataMap = std::vector<std::optional<std::pair<WordsArray, TagsArray>>>;

RawArray buildRawDictFromXML(const std::string& path);
