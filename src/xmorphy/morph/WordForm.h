#pragma once
#include <xmorphy/graphem/Token.h>
#include <xmorphy/tag/AnalyzerTag.h>
#include <xmorphy/tag/ITag.h>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>

#include <unordered_set>
namespace X
{
struct MorphInfo
{
    utils::UniString normalForm;
    UniSPTag sp;
    UniMorphTag tag;
    mutable double probability;
    AnalyzerTag at;
    std::size_t stemLen;
    bool operator<(const MorphInfo & other) const;
    bool operator==(const MorphInfo & other) const;
    bool operator!=(const MorphInfo & other) const { return !((*this) == other); }

    MorphInfo() = default;
    MorphInfo(const MorphInfo & o) = default;

    MorphInfo & operator=(const MorphInfo & o)
    {
        normalForm = o.normalForm;
        probability = o.probability;
        at = o.at;
        stemLen = o.stemLen;
        tag = o.tag;
        sp = o.sp;
        return *this;
    }
    MorphInfo(const utils::UniString & nf, const ITag & sp, const ITag & mt, double prob, AnalyzerTag at, std::size_t stemLen)
        : normalForm(nf)
        , sp(dynamic_cast<const UniSPTag &>(sp))
        , tag(dynamic_cast<const UniMorphTag &>(mt))
        , probability(prob)
        , at(at)
        , stemLen(stemLen)
    {
    }
};

} // namespace analyze

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<X::MorphInfo>
{
    std::size_t operator()(X::MorphInfo const & s) const
    {
        size_t h1 = 0;
        h1 += std::hash<utils::UniString>{}(s.normalForm);
        h1 ^= std::hash<size_t>{}(s.tag.getValue());
        h1 += std::hash<size_t>{}(s.sp.getValue());
        return h1;
    }
};
} // namespace std

namespace X
{
class WordForm : public Token
{
protected:
    std::unordered_set<MorphInfo> morphInfos;
    std::vector<PhemTag> phemInfo;
    using Token::getInner;

public:
    WordForm(
        const utils::UniString & wordForm_,
        const std::unordered_set<MorphInfo> & morphInfos_,
        TokenTypeTag t = TokenTypeTag::UNKN,
        GraphemTag tt = GraphemTag::UNKN)
        : Token(wordForm_, t, tt), morphInfos(morphInfos_)
    {
        if (morphInfos.empty())
        {
            this->morphInfos.insert(MorphInfo{utils::UniString("?"), UniSPTag::X, UniMorphTag::UNKN, 1.0, AnalyzerTag::UNKN, false});
        }
    }
    const utils::UniString & getWordForm() const { return Token::getInner(); }

    const std::unordered_set<MorphInfo> & getMorphInfo() const { return morphInfos; }
    std::unordered_set<MorphInfo> & getMorphInfo() { return morphInfos; }
    void setMorphInfo(const std::unordered_set<MorphInfo> & mi) { morphInfos = mi; }
    void setPhemInfo(const std::vector<PhemTag> & phems) { phemInfo = phems; }
    std::vector<PhemTag> getPhemInfo() const { return phemInfo; }

    std::vector<PhemTag> & getPhemInfo() { return phemInfo; }
};

using WordFormPtr = std::shared_ptr<WordForm>;

using Sentence = std::vector<WordFormPtr>;
}
