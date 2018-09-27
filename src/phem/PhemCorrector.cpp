#include <phem/PhemCorrector.h>
namespace phem {

    void PhemCorrector::correct(analyze::WordFormPtr wf) const {
        removeImpossiblePrefix(wf);
    }

    void PhemCorrector::removeImpossiblePrefix(analyze::WordFormPtr wf) const {
        size_t prefixSize = 0;
        auto& phemInfo = wf->getPhemInfo();
        const auto& word = wf->getWordForm();
        for (const auto& tag : phemInfo) {
            if (tag != base::PhemTag::PREFIX)
                break;
            else
                prefixSize++;
        }

        if (prefixSize == 0)
            return;

        auto prefix = word.cut(prefixSize);

        while (!prefDict.count(prefix) && !prefix.isEmpty()) {
            prefix = prefix.cut(prefix.length() - 1);
        }

        for (size_t i = 0; i < phemInfo.size(); ++i) {
            if (i < prefix.length()) {
                phemInfo[i] = base::PhemTag::PREFIX;
            } else if (phemInfo[i] == base::PhemTag::PREFIX) {
                phemInfo[i] = base::PhemTag::ROOT;
            } else {
                break;
            }
        }
    }
} // namespace phem
