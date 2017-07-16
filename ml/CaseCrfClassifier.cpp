#include "CaseCrfClassifier.h"
namespace ml {

std::array<std::string, CASE_INPUT_SIZE> getCaseFreautres(analyze::WordFormPtr data) {
    std::array<std::string, CASE_INPUT_SIZE> result;
    std::array<std::size_t, base::MorphTag::caseSize()> features = {{0}};
    for (auto info : data->getMorphInfo()) {
        base::MorphTag cas = info.tag.getCase();
        if (cas != base::MorphTag::UNKN) {
            features[base::MorphTag::getCase(cas)] = 1;
        }
        result[1] = to_string(info.sp);              // All are same
        result[2] = to_string(info.tag.getGender()); //Also all same
        result[3] = to_string(info.tag.getNumber()); //All are same
    }
    result[0] = data->getWordForm().toUpperCase().getRawString();
    for (std::size_t j = 4, i = 0; i < base::MorphTag::caseSize(); ++j, ++i) {
        result[j] = std::to_string(features[i]);
    }
    return result;
}

std::vector<base::MorphTag> CaseCRFClassifier::classify(const std::vector<analyze::WordFormPtr>& wf) const {
    std::vector<std::array<std::string, CASE_INPUT_SIZE>> features(wf.size());
    std::transform(wf.begin(), wf.end(), features.begin(), [](analyze::WordFormPtr p) { return getCaseFreautres(p); });
    std::vector<std::string> strRes = classifyBase(features);
    std::vector<base::MorphTag> result(wf.size());
    std::transform(strRes.begin(), strRes.end(), result.begin(), [](const std::string& str) { return base::MorphTag(str); });
    return result;
}
}
