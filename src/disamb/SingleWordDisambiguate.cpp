#include "SingleWordDisambiguate.h"
#include <incbin.h>

namespace disamb {

namespace
{
    INCBIN(disambdict, "dicts/uddisambdict.bin");
}

SingleWordDisambiguate::SingleWordDisambiguate() {
    std::istringstream disamb_is(std::string{reinterpret_cast<const char*>(gdisambdictData), gdisambdictSize});
    dict = build::DisambDict::loadFromFiles(disamb_is);
}

void SingleWordDisambiguate::disambiguate(std::vector<analyze::WordFormPtr>& seq) const {
    for (analyze::WordFormPtr wf : seq) {
        disambiguateSingleForm(wf);
    }
}

void SingleWordDisambiguate::disambiguateSingleForm(analyze::WordFormPtr form) const
{
    double sum = 0;
    std::unordered_set<analyze::MorphInfo>& infos = form->getMorphInfo();
    std::vector<size_t> counts;
    for (auto itr = infos.begin(); itr != infos.end(); ++itr) {
        base::UniMorphTag mt = itr->tag;
        base::UniSPTag sp = itr->sp;
        std::size_t count = dict->getCount(form->getWordForm(), sp, mt);
        sum += count;
        counts.push_back(count);
    }
    if (sum > 0) {
        size_t i = 0;
        for (auto itr = infos.begin(); itr != infos.end(); ++itr, ++i) {
            itr->probability = counts[i] / sum;
        }
    }
}
}
