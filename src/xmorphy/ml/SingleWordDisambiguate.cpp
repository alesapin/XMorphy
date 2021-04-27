#include <xmorphy/ml/SingleWordDisambiguate.h>

#include <incbin.h>

namespace X
{
namespace
{
    INCBIN(disambdict, "dicts/uddisambdict.bin");
}

SingleWordDisambiguate::SingleWordDisambiguate()
{
    std::istringstream disamb_is(std::string{reinterpret_cast<const char *>(gdisambdictData), gdisambdictSize});
    dict = DisambDict::loadFromFiles(disamb_is);
}

void SingleWordDisambiguate::disambiguate(std::vector<WordFormPtr> & seq) const
{
    for (WordFormPtr wf : seq)
    {
        disambiguateSingleForm(wf);
    }
}

void SingleWordDisambiguate::disambiguateSingleForm(WordFormPtr form) const
{
    double sum = 0;
    std::unordered_set<MorphInfo> & infos = form->getMorphInfo();
    std::vector<size_t> counts;
    /// Participles is always prefered among NOUN and ADJ, except short
    size_t parts_count = 0;
    /// Short ADJ is always prefered among other variants
    size_t short_adjs = 0;
    for (auto itr = infos.begin(); itr != infos.end(); ++itr)
    {
        UniMorphTag mt = itr->tag;
        UniSPTag sp = itr->sp;
        if (mt & UniMorphTag::Part && sp == UniSPTag::VERB)
            parts_count++;

        if (mt & UniMorphTag::Short && sp == UniSPTag::ADJ)
            short_adjs++;

        size_t count = dict->getCount(form->getWordForm(), sp, mt);
        sum += count;
        counts.push_back(count);
    }

    if (sum > 0 || parts_count != 0 || short_adjs != 0)
    {
        size_t i = 0;
        for (auto itr = infos.begin(); itr != infos.end(); ++itr, ++i)
        {
            if (short_adjs != 0)
            {
                if (itr->tag & UniMorphTag::Short && itr->sp == UniSPTag::ADJ)
                    itr->probability = 1.0 / short_adjs;
                else
                    itr->probability = 0;
            }
            else if (parts_count != 0)
            {
                if (itr->tag & UniMorphTag::Part && itr->sp == UniSPTag::VERB)
                    itr->probability = 1.0 / parts_count;
                else
                    itr->probability = 0;
            }
            else
                itr->probability = counts[i] / sum;
        }
    }
}
}
