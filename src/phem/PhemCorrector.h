#pragma once
#include <utils/UniString.h>
#include <set>
#include <vector>
#include <tag/PhemTag.h>
#include <morph/WordForm.h>

namespace phem {
    class PhemCorrector {
    public:
        PhemCorrector(const std::set<utils::UniString>& prefixDict)
            : prefDict(prefixDict) {
        }
        void correct(analyze::WordFormPtr wf) const;

    private:
        std::set<utils::UniString> prefDict;
        void removeImpossiblePrefix(analyze::WordFormPtr wf) const;
    };
} // namespace phem
