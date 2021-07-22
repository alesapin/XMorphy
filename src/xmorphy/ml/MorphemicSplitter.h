#pragma once

#include <memory>
#include <xmorphy/ml/KerasModel.h>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/build/PhemDict.h>
#include <xmorphy/ml/KerasMultiModel.h>
#include <xmorphy/utils/LRUCache.h>

namespace X
{


class MorphemicSplitter
{
private:

    std::unique_ptr<KerasMultiModel> model;

    std::unique_ptr<PhemDict> phem_dict;

    struct CacheKey
    {
        UniString word;
        UniSPTag sp;
        UniMorphTag tag;
        bool operator==(const CacheKey & other) const
        {
            return std::tie(word, sp, tag) == std::tie(other.word, other.sp, other.tag);
        }
    };

    struct CacheKeyHasher
    {
        size_t operator()(CacheKey const & s) const noexcept
        {
            size_t result{};
            result = std::hash<UniString>{}(s.word);
            boost::hash_combine(result, std::hash<ITag>{}(s.sp));
            boost::hash_combine(result, std::hash<ITag>{}(s.tag));
            return result;
        }
    };

    mutable LRUCache<CacheKey, std::vector<PhemTag>, CacheKeyHasher> lru_cache;

public:
    MorphemicSplitter();

    void split(WordFormPtr form) const;

    std::vector<PhemTag> split(const UniString & word, UniSPTag sp, UniMorphTag tag) const;
};

}
