#include <ml/Disambiguator.h>

namespace ml {
std::vector<analyze::MorphInfo> Disambiguator::disambiguate(const analyze::Sentence & forms) const {
    if (forms.size() == 0)
        return {};
    std::vector<analyze::MorphInfo> result;
    result.resize(forms.size());

    std::vector<float> features;
    size_t one_form_size = 0;
    for (const auto & wf : forms)
    {
        auto em = embedding.getWordVector(wf);
        auto sp_features = sp_classifier.getVectorWithPossibleParts(wf);
        one_form_size = em.size() + sp_features.size();
        features.insert(features.end(), em.begin(), em.end());
        features.insert(features.end(), sp_features.begin(), sp_features.end());
    }
    if (forms.size() < sequence_size)
        features.insert(features.end(), one_form_size * (sequence_size - forms.size()), 0);
    auto sps = sp_classifier.classifySpeechPart(features);
    for (size_t i = 0; i < sps.size(); ++i) {
        result[i].sp = sps[i];
    }

    return result;
}
} // namespace ml
