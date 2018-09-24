#ifndef _NORMAL_FORM_CRF_CLASSIFIER
#define _NORMAL_FORM_CRF_CLASSIFIER
#include "CRF.h"
#include <tag/SpeechPartTag.h>
#include <morph/WordForm.h>
#include "CaseCrfClassifier.h"

namespace ml {

static const std::size_t NF_INPUT_SIZE = 5 + 3; //form and speechpart and gender and number and case + three variants
std::array<std::string, NF_INPUT_SIZE> getNormalFormFeatures(analyze::WordFormPtr data);

class NormalFormCRFClassifier : CRF<NF_INPUT_SIZE> {
public:
    using CRF::CRF;
    std::vector<utils::UniString> classify(const std::vector<analyze::WordFormPtr>& wf) const;
};
}
#endif
