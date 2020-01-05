#include "DisambDict.h"
namespace build {
std::size_t DisambDict::getCount(const utils::UniString& word, base::UniSPTag sp, base::UniMorphTag mt) const {
    std::string upCaseWord = word.toUpperCase().getRawString();
    std::vector<std::string> keys = dict->completeKey(upCaseWord);
    std::size_t result = 0;
    std::size_t maxCount = 0;
    for (const std::string& key : keys) {
        std::vector<std::string> parts;
        boost::split(parts, key, boost::is_any_of(DISAMBIG_SEPARATOR));
        if (parts[0] != upCaseWord)
            continue;
        base::UniSPTag candidateSp = base::UniSPTag::X;
        from_raw_string(parts[1], candidateSp);
        if (candidateSp != sp) {
            continue;
        }
        base::UniMorphTag candidateMt = base::UniMorphTag::UNKN;
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

std::unique_ptr<DisambDict> DisambDict::loadFromFiles(std::istream & is) {
    DisambDictPtr dct = std::make_shared<dawg::Dictionary<std::size_t>>();
    dct->deserialize(is);
    return utils::make_unique<DisambDict>(dct);
}
}
