#ifndef _CASE_CRF_CLASSIFIER
#define _CASE_CRF_CLASSIFIER
#include "CRF.h"
#include <tag/SpeechPartTag.h>
#include <morph/WordForm.h>
#include "NumberClassifier.h"

namespace ml {

static const std::size_t CASE_INPUT_SIZE = base::MorphTag::caseSize() + 4; //form and speechpart and gender and number
std::array<std::string, CASE_INPUT_SIZE> getCaseFreautres(analyze::WordFormPtr data);
class CaseCRFGenerator : public ITrainGenerator {
public:
    CaseCRFGenerator(const analyze::Processor& proc, const tokenize::Tokenizer& tok, const base::OpCorporaUDConverter& conv, TagType tt)
        : ITrainGenerator(proc, tok, conv, "CASE", tt) {
    }
    base::ITag getTag(base::ITag& sp, base::ITag& mt) const override {
        if (tt == TagType::UTAG) {
            base::UniMorphTag &umt = dynamic_cast<base::UniMorphTag&>(mt);
            return static_cast<base::ITag>(umt.getCase());
        } else {
            base::MorphTag opmt = base::MorphTag::UNKN;
            from_string(to_string(mt), opmt);
            return static_cast<base::ITag>(opmt.getCase());
        }
    }
};

class CaseCRFClassifier : CRF<CASE_INPUT_SIZE> {
public:
    using CRF::CRF;
    std::vector<base::MorphTag> classify(const std::vector<analyze::WordFormPtr>& wf) const;
};
}
#endif
