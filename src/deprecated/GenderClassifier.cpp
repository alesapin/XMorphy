#include "GenderClassifier.h"
namespace ml {

std::array<std::string, GENDER_INPUT_SIZE> getGenderFreautres(analyze::WordFormPtr data) {
    std::array<std::string, GENDER_INPUT_SIZE> result;
    std::array<std::size_t, base::MorphTag::genderSize()> features = {{0}};
    for (auto info : data->getMorphInfo()) {
        base::MorphTag gender = info.tag.getGender();
        if (gender != base::MorphTag::UNKN) {
            features[base::MorphTag::getGen(gender)] = 1;
        }
        result[1] = to_string(info.sp); // All are same
    }
    result[0] = data->getWordForm().toUpperCase().getRawString();
    for (std::size_t j = 2, i = 0; i < base::MorphTag::genderSize(); ++j, ++i) {
        result[j] = std::to_string(features[i]);
    }
    return result;
}

std::vector<base::MorphTag> GenderCRFClassifier::classify(const std::vector<analyze::WordFormPtr>& wf) const {
    std::vector<std::array<std::string, GENDER_INPUT_SIZE>> features(wf.size());
    std::transform(wf.begin(), wf.end(), features.begin(), [](analyze::WordFormPtr p) { return getGenderFreautres(p); });
    std::vector<std::string> strRes = classifyBase(features);
    std::vector<base::MorphTag> result(wf.size());
    std::transform(strRes.begin(), strRes.end(), result.begin(), [](const std::string& str) { return base::MorphTag(str); });
    return result;
}
}
