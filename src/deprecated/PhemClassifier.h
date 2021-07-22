#ifndef _PHEM_CLASSIFIER_H
#define _PHEM_CLASSIFIER_H
#include "CRF.h"
#include <tag/SpeechPartTag.h>
#include <tag/MorphTag.h>
#include <tag/PhemTag.h>
#include <morph/WordForm.h>
#include <build/PhemDict.h>
#include <memory>
namespace ml {
static constexpr std::size_t LETTERS_SIZE = 1;
static constexpr std::size_t VOVEL_SIZE = 1;
static constexpr std::size_t WORD_SIZE = 1;
static constexpr std::size_t STEM_SIZE = 1;
static constexpr std::size_t SP_SIZE = 1;
static constexpr std::size_t GENDER_SIZE = 1;
static constexpr std::size_t NUMBER_SIZE = 1;
static constexpr std::size_t CASE_SIZE = 1;
static constexpr std::size_t TENSE_SIZE = 1;
static constexpr std::size_t HARRIS_SIZE = 2;
static constexpr std::size_t PHEM_FEATURES_SIZE = LETTERS_SIZE   /* for letters in russian */
                                                  + VOVEL_SIZE   // for word length
                                                  + WORD_SIZE    // for stem size
                                                  + STEM_SIZE    // for vocative
                                                  + SP_SIZE      // for speechpart
                                                  + GENDER_SIZE  // for genders
                                                  + NUMBER_SIZE  // for numbers
                                                  + CASE_SIZE    //for cases
                                                  + TENSE_SIZE   // for tenses
                                                  + HARRIS_SIZE; // Harris forwrad and backward numbers
static constexpr std::size_t PHEM_OUTPUT_SIZE = base::PhemTag::size();

using FeatureAnswerPair = std::vector<std::pair<analyze::WordFormPtr, std::vector<base::PhemTag>>>;
class PhemClassifier : public CRF<PHEM_FEATURES_SIZE> {
private:
    std::unique_ptr<build::PhemDict> dict;

public:
    PhemClassifier(std::unique_ptr<build::PhemDict>&& dict, const std::string& modelpath)
        : CRF(modelpath)
        , dict(std::move(dict)) {
    }

    PhemClassifier(std::unique_ptr<build::PhemDict>&& dict)
        : CRF()
        , dict(std::move(dict)) {
    }

    std::array<std::string, PHEM_FEATURES_SIZE>
    getPhemFreautres(analyze::WordFormPtr wf, const UniString& upCaseWf, std::size_t letterIndex) const;

    std::vector<std::array<std::string, PHEM_FEATURES_SIZE>> getPhemFreautres(analyze::WordFormPtr wf) const;

    void generate(const FeatureAnswerPair& trainData, std::ostream& result) const;
    void classify(analyze::WordFormPtr wf) const;
};
}
#endif
