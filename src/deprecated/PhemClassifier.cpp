#include "PhemClassifier.h"
namespace ml {

std::array<std::string, PHEM_FEATURES_SIZE>
PhemClassifier::getPhemFreautres(analyze::WordFormPtr wf, const UniString& upCaseWf, std::size_t letterIndex) const {
    std::size_t start = 0;
    std::array<std::string, PHEM_FEATURES_SIZE> result = {{""}};
    analyze::MorphInfo mi = *(wf->getMorphInfo().begin());

    utils::UniCharacter letter = upCaseWf[letterIndex];
    result[start] = letter.getInnerRepr();

    start += LETTERS_SIZE;

    if (utils::UniCharacter::VOWELS.count(letter)) {
        result[start] = std::to_string(1);
    } else {
        result[start] = std::to_string(0);
    }
    start += VOVEL_SIZE;
    result[start] = std::to_string(upCaseWf.length());

    start += WORD_SIZE;
    result[start] = std::to_string(mi.stemLen);

    start += STEM_SIZE;
    result[start] = to_string(mi.sp);

    start += SP_SIZE;
    result[start] = to_string(mi.tag.getGender());

    start += GENDER_SIZE;
    result[start] = to_string(mi.tag.getNumber());

    start += NUMBER_SIZE;

    result[start] = to_string(mi.tag.getCase());
    start += CASE_SIZE;

    result[start] = to_string(mi.tag.getTense());
    start += TENSE_SIZE;

    std::size_t forwardHarris = letterIndex == 0 ? 0 : dict->countPrefix(upCaseWf.subString(0, letterIndex));
    result[start] = std::to_string(forwardHarris);
    std::size_t backwardHarris = letterIndex == upCaseWf.length() - 1 ? 0 : dict->countSuffix(upCaseWf.rcut(upCaseWf.length() - letterIndex));
    result[start + 1] = std::to_string(backwardHarris);

    return result;
}

std::vector<std::array<std::string, PHEM_FEATURES_SIZE>> PhemClassifier::getPhemFreautres(analyze::WordFormPtr wf) const {
    UniString upCaseWf = wf->getWordForm().toUpperCase();
    std::vector<std::array<std::string, PHEM_FEATURES_SIZE>> result(upCaseWf.length());
    for (std::size_t i = 0; i < upCaseWf.length(); ++i) {
        result[i] = getPhemFreautres(wf, upCaseWf, i);
    }
    return result;
}

void PhemClassifier::generate(const FeatureAnswerPair& trainData, std::ostream& result) const {
    for (std::size_t i = 0; i < trainData.size(); ++i) {
        std::cerr << std::get<0>(trainData[i])->getWordForm() << "\n";
        auto featureRepr = getPhemFreautres(std::get<0>(trainData[i]));
        for (std::size_t j = 0; j < featureRepr.size(); ++j) {
            for (std::size_t k = 0; k < featureRepr[j].size(); ++k) {
                result << featureRepr[j][k] << "\t";
            }
            result << std::get<1>(trainData[i])[j] << "\n";
        }
        result << "\n";
    }
}

void PhemClassifier::classify(analyze::WordFormPtr wf) const {
    if (false && dict->contains(wf->getWordForm())) {
        wf->setPhemInfo(dict->getPhemParse(wf->getWordForm()));
    } else {
        std::vector<std::array<std::string, PHEM_FEATURES_SIZE>> features = getPhemFreautres(wf);

        std::vector<std::string> strRes = classifyBase(features);
        std::vector<base::PhemTag> result;
        for (const auto& str : strRes) {
            result.push_back(base::PhemTag(str));
        }
        wf->setPhemInfo(result);
    }
}
}
