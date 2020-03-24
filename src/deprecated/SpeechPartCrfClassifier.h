#ifndef _SPEECH_PART_CRF_CLASSIFIER
#define _SPEECH_PART_CRF_CLASSIFIER
#include "CRF.h"
#include <tag/SpeechPartTag.h>
#include <morph/WordForm.h>
#include <tag/OpCorporaUDConverter.h>
#include "ITrainGenerator.h"
#include <tuple>
namespace ml {

static const std::size_t INPUT_SIZE = base::SpeechPartTag::size();
std::array<std::string, INPUT_SIZE> getFreautres(analyze::WordFormPtr wf);

class SpeechPartCRFGenerator : public ITrainGenerator {
protected:
public:
    SpeechPartCRFGenerator(const analyze::Processor& proc, const tokenize::Tokenizer& tok, const base::OpCorporaUDConverter& conv, TagType tt)
        : ITrainGenerator(proc, tok, conv, "SpeechPart", tt) {
    }

    base::ITag getTag(base::ITag &sp, base::ITag &mt) const override {
        return sp;
    }
};

class SpeechPartCRFClassifier : public CRF<INPUT_SIZE> {
public:
    using CRF::CRF;
    std::vector<base::SpeechPartTag> classify(const std::vector<analyze::WordFormPtr>& wf) const;
};
}
#endif
