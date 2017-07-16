#ifndef _NUMBER_CLASSIFIER_H
#define _NUMBER_CLASSIFIER_H
#include "GenderClassifier.h"
namespace ml {

static const std::size_t NUMBER_INPUT_SIZE = base::MorphTag::numberSize() + 3; // base::MorphTag::numberSize() + 3; //form and speechpart and gender
std::array<std::string, NUMBER_INPUT_SIZE> getNumberFreautres(analyze::WordFormPtr data);
class NumberCRFGenerator : public ITrainGenerator {
public:
    NumberCRFGenerator(const analyze::Processor& proc, const tokenize::Tokenizer& tok, const base::OpCorporaUDConverter& conv, TagType tt)
        : ITrainGenerator(proc, tok, conv, "NUMBER", tt) {
    }
    base::ITag getTag(base::ITag& sp, base::ITag& mt) const override {
        if (tt == TagType::UTAG) {
            base::UniMorphTag& umt = dynamic_cast<base::UniMorphTag&>(mt);
            return static_cast<base::ITag>(umt.getNumber());
        } else {
            base::MorphTag opmt = base::MorphTag::UNKN;
            from_string(to_string(mt), opmt);
            return static_cast<base::ITag>(opmt.getNumber());
        }
    }
};

class NumberCRFClassifier : CRF<GENDER_INPUT_SIZE> {
public:
    using CRF::CRF;
    std::vector<base::MorphTag> classify(const std::vector<analyze::WordFormPtr>& wf) const;
};
}
#endif
