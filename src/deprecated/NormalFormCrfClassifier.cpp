#include "NormalFormCrfClassifier.h"
namespace ml {

std::array<std::string, NF_INPUT_SIZE> getNormalFormFeatures(analyze::WordFormPtr data) {
    std::array<std::string, NF_INPUT_SIZE> result;
    std::array<std::string, 3> features;
    std::size_t i = 0;
    for (auto info : data->getMorphInfo()) {
        features[i++] = info.normalForm.getRawString();
        result[1] = to_string(info.usp);              // All are same
        result[2] = to_string(info.utag.getGender()); //Also all same
        result[3] = to_string(info.utag.getNumber());
        result[4] = to_string(info.utag.getCase());
        if (i > 3) {
            break;
        }
    }
    result[0] = data->getWordForm().toUpperCase().getRawString();
    for (std::size_t j = 5, i = 0; i < 3; ++j, ++i) {
        if (features[i].empty()) {
            result[j] = "_";
        } else {
            result[j] = features[i];
        }
    }
    return result;
}

std::vector<utils::UniString> NormalFormCRFClassifier::classify(const std::vector<analyze::WordFormPtr>& wf) const {
    std::vector<std::array<std::string, NF_INPUT_SIZE>> features(wf.size());
    std::transform(wf.begin(), wf.end(), features.begin(), [](analyze::WordFormPtr p) { return getNormalFormFeatures(p); });
    std::vector<std::string> strRes = classifyBase(features);
    std::vector<std::size_t> resultNum(wf.size());
    std::transform(strRes.begin(), strRes.end(), resultNum.begin(), [](const std::string& str) { return std::stoul(str); });
    std::vector<utils::UniString> res(wf.size());
    for (std::size_t i = 0; i < wf.size(); ++i) {
        auto &infos = wf[i]->getMorphInfo();
        std::size_t pos = resultNum[i] % infos.size();
        res[i] = std::next(infos.begin(), pos)->normalForm;
    }
    return res;
}
}
