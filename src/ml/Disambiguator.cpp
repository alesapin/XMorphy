#include <ml/Disambiguator.h>

namespace ml {
std::vector<analyze::MorphInfo> Disambiguator::disambiguate(const analyze::Sentence & forms) const {
    std::cerr << "FORMS:" << forms.size() << std::endl;
    if (forms.size() != sequence_size)
        throw std::runtime_error {"Sequence size " + std::to_string(forms.size()) + " is not equal to required " + std::to_string(forms.size())};
    std::vector<analyze::MorphInfo> result;
    result.resize(forms.size());

    std::vector<float> features;
    for (const auto & wf : forms)
    {
        auto em = embedding.getWordVector(wf);
        auto sp_features = sp_classifier.getVectorWithPossibleParts(wf);
        features.insert(features.end(), em.begin(), em.end());
        features.insert(features.end(), sp_features.begin(), sp_features.end());
    }
    auto sps = sp_classifier.classifySpeechPart(features);
    for (size_t i = 0; i < sps.size(); ++i) {
        result[i].sp = sps[i];
    }

    return result;
}
} // namespace ml
