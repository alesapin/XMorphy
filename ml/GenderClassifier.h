#ifndef _GENDER_CLASSIFIER_H
#define _GENDER_CLASSIFIER_H
#include "SpeechPartCrfClassifier.h"
#include "../tag/UniSPTag.h"
#include "../tag/MorphTag.h"
namespace ml {
static const std::size_t GENDER_INPUT_SIZE = base::MorphTag::genderSize() + 2; //form and speechpart

std::array<std::string, GENDER_INPUT_SIZE> getGenderFreautres(analyze::WordFormPtr data);
class GenderCRFGenerator : public ITrainGenerator {
public:
    GenderCRFGenerator(const analyze::Processor& proc, const tokenize::Tokenizer& tok, const base::OpCorporaUDConverter& conv, TagType tt)
        : ITrainGenerator(proc, tok, conv, "GENDER", tt) {
    }

    base::ITag getTag(base::ITag& sp, base::ITag& mt) const override {
        if (tt == TagType::UTAG) {
            base::UniMorphTag& umt = dynamic_cast<base::UniMorphTag&>(mt);
            return static_cast<base::ITag>(umt.getGender());
        } else {
            base::MorphTag opmt = base::MorphTag::UNKN;
            from_string(to_string(mt), opmt);
            return static_cast<base::ITag>(opmt.getGender());
        }
    }
};

class GenderCRFClassifier : CRF<GENDER_INPUT_SIZE> {
public:
    using CRF::CRF;
    std::vector<base::MorphTag> classify(const std::vector<analyze::WordFormPtr>& wf) const;
};
}
#endif
