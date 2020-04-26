#include "SingleWordDisambiguate.h"
#include <incbin.h>

namespace X {

namespace
{
    INCBIN(disambdict, "dicts/uddisambdict.bin");
}

SingleWordDisambiguate::SingleWordDisambiguate() {
    std::istringstream disamb_is(std::string{reinterpret_cast<const char*>(gdisambdictData), gdisambdictSize});
    dict = DisambDict::loadFromFiles(disamb_is);
}

void SingleWordDisambiguate::disambiguate(std::vector<WordFormPtr>& seq) const {
    for (WordFormPtr wf : seq) {
        disambiguateSingleForm(wf);
    }
}

void SingleWordDisambiguate::disambiguateSingleForm(WordFormPtr form) const
{
    double sum = 0;
    std::unordered_set<MorphInfo>& infos = form->getMorphInfo();
    std::vector<size_t> counts;
    for (auto itr = infos.begin(); itr != infos.end(); ++itr) {
        UniMorphTag mt = itr->tag;
        UniSPTag sp = itr->sp;
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
