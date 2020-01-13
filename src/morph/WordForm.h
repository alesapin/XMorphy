#ifndef _WORD_FORM_H
#define _WORD_FORM_H
#include <graphem/Token.h>
#include <tag/AnalyzerTag.h>
#include <tag/ITag.h>
#include <tag/UniSPTag.h>
#include <tag/UniMorphTag.h>
#include <tag/PhemTag.h>

#include <set>
namespace analyze {
struct MorphInfo {
    utils::UniString normalForm;
    base::UniMorphTag tag;
    base::UniSPTag sp;
    mutable double probability;
    base::AnalyzerTag at;
    std::size_t stemLen;
    bool operator<(const MorphInfo& other) const;
    bool operator==(const MorphInfo& other) const;
    bool operator!=(const MorphInfo& other) const {
        return !((*this) == other);
    }

    MorphInfo(const MorphInfo& o)
        : normalForm(o.normalForm)
        , tag(o.tag)
        , sp(o.sp)
        , probability(o.probability)
        , at(o.at)
        , stemLen(o.stemLen) {
    }
    MorphInfo& operator=(const MorphInfo& o) {
        normalForm = o.normalForm;
        probability = o.probability;
        at = o.at;
        stemLen = o.stemLen;
        tag = o.tag;
        sp = o.sp;
        return *this;
    }
    MorphInfo(
        const utils::UniString& nf, const base::ITag& sp, const base::ITag& mt, double prob, base::AnalyzerTag at, std::size_t stemLen)
        : normalForm(nf)
        , probability(prob)
        , at(at)
        , stemLen(stemLen)
    {
        this->tag = dynamic_cast<const base::UniMorphTag&>(mt);
        this->sp = dynamic_cast<const base::UniSPTag&>(sp);
    }
};

class WordForm : public base::Token {
protected:
    std::set<MorphInfo> morphInfos;
    std::vector<base::PhemTag> phemInfo;
    using base::Token::getInner;

public:
    WordForm(const utils::UniString& wordForm,
             const std::set<MorphInfo>& morphInfos,
             base::TokenTypeTag t = base::TokenTypeTag::UNKN,
             base::GraphemTag tt = base::GraphemTag::UNKN)
        : Token(wordForm, t, tt)
        , morphInfos(morphInfos) {
        if (morphInfos.empty()) {
            this->morphInfos.insert(MorphInfo{utils::UniString("?"), base::UniSPTag::X, base::UniMorphTag::UNKN, 1.0, base::AnalyzerTag::UNKN, false});
        }
    }
    virtual const utils::UniString& getWordForm() const {
        return Token::getInner();
    }
    utils::UniString toString() const override;
    virtual std::set<MorphInfo> getMorphInfo() const {
        return morphInfos;
    }
    virtual std::set<MorphInfo>& getMorphInfo() {
        return morphInfos;
    }
    virtual void setMorphInfo(const std::set<MorphInfo>& mi) {
        morphInfos = mi;
    }
    virtual void setPhemInfo(const std::vector<base::PhemTag>& phems) {
        phemInfo = phems;
    }
    virtual std::vector<base::PhemTag> getPhemInfo() const {
        return phemInfo;
    }

    virtual std::vector<base::PhemTag>& getPhemInfo() {
        return phemInfo;
    }
};

using WordFormPtr = std::shared_ptr<WordForm>;
}
#endif
