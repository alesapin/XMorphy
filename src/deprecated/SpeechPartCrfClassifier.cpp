#include "SpeechPartCrfClassifier.h"
namespace ml {

std::vector<base::SpeechPartTag> SpeechPartCRFClassifier::classify(const std::vector<analyze::WordFormPtr>& wf) const {
    std::vector<std::array<std::string, INPUT_SIZE>> features(wf.size());
    std::transform(wf.begin(), wf.end(), features.begin(), [](analyze::WordFormPtr p) { return getFreautres(p); });
    std::vector<std::string> strRes = classifyBase(features);
    std::vector<base::SpeechPartTag> result(wf.size());
    std::transform(strRes.begin(), strRes.end(), result.begin(), [](const std::string& str) { return base::SpeechPartTag(str); });
    return result;
}

std::array<std::string, INPUT_SIZE> getFreautres(analyze::WordFormPtr data) {
    std::array<std::string, INPUT_SIZE> result;
    std::array<std::size_t, base::SpeechPartTag::size()> features = {{0}};
    for (auto info : data->getMorphInfo()) {
        features[base::SpeechPartTag::get(info.sp)] = 1;
    }
    result[0] = data->getWordForm().toUpperCase().getRawString();
    for (std::size_t j = 1; j < base::SpeechPartTag::size(); ++j) {
        result[j] = std::to_string(features[j]);
    }
    return result;
}
}
