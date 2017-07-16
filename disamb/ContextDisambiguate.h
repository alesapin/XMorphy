#ifndef _CONTEXT_DISAMBIGUATE_H
#define _CONTEXT_DISAMBIGUATE_H
#include "IDisambig.h"
#include "../ml/SpeechPartCrfClassifier.h"
#include "../ml/GenderClassifier.h"
#include "../ml/NumberClassifier.h"
#include "../ml/CaseCrfClassifier.h"
#include <functional>
namespace disamb {
class ContextDisambiguator : IDisambig {
    ml::SpeechPartCRFClassifier spcls;
    ml::GenderCRFClassifier gencls;
    ml::NumberCRFClassifier numcls;
    ml::CaseCRFClassifier casecls;

    void filterInfos(analyze::WordFormPtr wf, std::function<bool(const analyze::MorphInfo&)> filt, std::function<void(analyze::MorphInfo&)> apply) const;

public:
    ContextDisambiguator(const std::string& spmodel, const std::string& genmodel, const std::string& nummodel, const std::string& casemodel)
        : spcls(spmodel)
        , gencls(genmodel)
        , numcls(nummodel)
        , casecls(casemodel) {
    }
    void disambiguate(std::vector<analyze::WordFormPtr>& sequence) const override;
};
}
#endif
