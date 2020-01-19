#include "DisambDict.h"
namespace build {
std::size_t DisambDict::getCount(
    const utils::UniString& word,
    base::UniSPTag sp,
    base::UniMorphTag mt) const
{
    std::string upCaseWord = word.toUpperCase().getRawString();
    std::vector<std::string> keys = dict->completeKey(upCaseWord);
    std::size_t count = 0;
    std::size_t maxSameBits = 0;
    for (const std::string& key : keys) {
        std::vector<std::string> parts;
        size_t i;
        size_t len = 0;
        size_t prev = 0;
        for (i = 0; i < key.size(); ++i)
        {
            if (key[i] == DISAMBIG_SEPARATOR)
            {
                parts.emplace_back(key.substr(prev, len));
                len = 0;
                prev = i + 1;
            } else {
                ++len;
            }
        }
        if (prev < key.size() && len > 0)
            parts.emplace_back(key.substr(prev, len));

        if (parts[0] != upCaseWord)
        {
            continue;
        }
        base::UniSPTag candidateSp = base::UniSPTag::X;
        from_raw_string(parts[1], candidateSp);
        if (candidateSp != sp) {
            continue;
        }
        base::UniMorphTag candidateMt = base::UniMorphTag::UNKN;
        from_raw_string(parts[2], candidateMt);
        std::size_t sameBits = count_intersection(mt, candidateMt);
        std::size_t currentKeyCount = dict->getValue(key);
        if (sameBits > maxSameBits) {
            maxSameBits = sameBits;
            count = currentKeyCount;
        } else if (sameBits == maxSameBits && currentKeyCount > count) {
            count = currentKeyCount;
        }
    }
    return count;
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
