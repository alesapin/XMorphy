#pragma once

#include <ml/CatBoost.h>
#include <tag/SpeechPartTag.h>
#include <tag/MorphTag.h>
#include <tag/PhemTag.h>
#include <morph/WordForm.h>
#include <build/PhemDict.h>
#include <memory>

//(0, 'н'), # letter itself (cat)
//(1, 'CONSONANT'), # vowel or consonant (cat)
//(2, 0), # index in word (num)
//(3, 67000), # CIPH law (num)
//(4, ''), # word[index-3] (cat)
//(5, ''), # word[index-2] (cat)
//(6, ''), # word[index-1] (cat)
//(7, 'а'),# word[index+1] (cat)
//(8, 'в'),# word[index+2] (cat)
//(9, 'л'),# word[index+3] (cat)
//(10, 'PRTF'),  # speech part (cat)
//(11, 'nomn'), # case         (cat)
//(12, 'masc'), # gender       (cat)
//(13, 'sing'), # number       (cat)
//(14, 'past'), # tense        (cat)
//(15, 11),     # word length  (num)
//(16, 11)      # stem length  (num)
// #17, 18 # Harris law         (num)

namespace ml {
    static constexpr std::size_t PHEM_OUTPUT_SIZE = base::PhemTag::size() - 1;

    class PhemCatBoostClassifier : public CatBoostWrapper<4, 13, PHEM_OUTPUT_SIZE> {
    private:
        std::unique_ptr<build::PhemDict> dict;

        std::pair<NumFeatures, CatFeatures> getPhemFreautres(analyze::WordFormPtr wf, const utils::UniString& lowerCaseWf, std::size_t letterIndex) const;

        std::pair<std::vector<NumFeatures>, std::vector<CatFeatures>> getPhemFreautres(analyze::WordFormPtr wf) const;

    public:
        PhemCatBoostClassifier(std::unique_ptr<build::PhemDict> dict, const std::string& modelPath)
            : CatBoostWrapper(modelPath)
            , dict(std::move(dict)) {
        }

        void classify(analyze::WordFormPtr wf) const;
    };
} // namespace ml
