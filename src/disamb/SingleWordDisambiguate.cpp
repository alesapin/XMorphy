#include "SingleWordDisambiguate.h"
namespace disamb {
void SingleWordDisambiguate::disambiguate(std::vector<analyze::WordFormPtr>& seq) const {
    for (analyze::WordFormPtr wf : seq) {
        std::size_t sum = 0;
        std::set<analyze::MorphInfo>& infos = wf->getMorphInfo();
        for (auto itr = infos.begin(); itr != infos.end(); ++itr) {
            utils::UniString word = itr->normalForm.toUpperCase();
            base::UniMorphTag mt = itr->tag;
            base::UniSPTag sp = itr->sp;
            std::size_t count = dict->getCount(word, sp, mt);
            sum += count;
        }
        if (sum > 0) {
            for (auto itr = infos.begin(); itr != infos.end(); ++itr) {
                utils::UniString word = itr->normalForm.toUpperCase();
                base::UniMorphTag mt = itr->tag;
                base::UniSPTag sp = itr->sp;
                std::size_t count = dict->getCount(word, sp, mt);
                itr->probability = (double)count / sum;
            }
        }
        analyze::MorphInfo finalInfo = *std::max_element(infos.begin(), infos.end(), [](const analyze::MorphInfo& mi1, const analyze::MorphInfo& mi2) { return mi1.probability < mi2.probability; });
        infos.clear();
        infos.insert(finalInfo);
    }
}
}
