#include "DisambDict.h"
namespace build {
std::size_t DisambDict::getCount(const utils::UniString& word, base::SpeechPartTag sp, base::MorphTag mt) const {
    std::string upCaseWord = word.toUpperCase().getRawString();
    std::vector<std::string> keys = dict->completeKey(upCaseWord);
    std::size_t result = 0;
    std::size_t maxCount = 0;
    for (const std::string& key : keys) {
        std::vector<std::string> parts;
        boost::split(parts, key, boost::is_any_of(DISAMBIG_SEPARATOR));
        if (parts[0] != upCaseWord)
            continue;
        base::SpeechPartTag candidateSp = base::SpeechPartTag::UNKN;
        from_raw_string(parts[1], candidateSp);
        if (candidateSp != sp) {
            continue;
        }
        base::MorphTag candidateMt = base::MorphTag::UNKN;
        from_raw_string(parts[2], candidateMt);
        std::size_t sameBits = count_intersection(mt, candidateMt);
        std::size_t value = dict->getValue(key);
        if (sameBits > maxCount) {
            maxCount = sameBits;
            result = value;
        } else if (sameBits == maxCount && value > result) {
            result = value;
        }
    }
    return result;
}

void dropToFiles(const std::unique_ptr<DisambDict>& dct, const std::string& filename) {
    std::ofstream ofs(filename);
    dct->dict->serialize(ofs);
}

void loadFromFiles(std::unique_ptr<DisambDict>& dict, const std::string& filename) {
    std::ifstream ifs(filename);
    DisambDictPtr dct = std::make_shared<dawg::Dictionary<std::size_t>>();
    dct->deserialize(ifs);
    dict = utils::make_unique<DisambDict>(dct);
}
}
