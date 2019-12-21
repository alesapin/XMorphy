#ifndef _PHEMMER_H
#define _PHEMMER_H
#include <build/PhemDict.h>
#include <build/DictBuilder.h>
#include <morph/WordForm.h>
#include <ml/PhemCatBoostClassifier.h>
#include <phem/PhemCorrector.h>
#include <vector>
#include <memory>

namespace phem {
class Phemmer {
    std::unique_ptr<ml::PhemCatBoostClassifier> classifier;
    std::unique_ptr<PhemCorrector> corrector;

public:
    Phemmer(
        std::istream & dictIs,
        std::istream & forwardIs,
        std::istream & backwardIs,
        std::istream & prefDictIs,
        const std::string & libPath,
        const std::string& modelpath) 
    {
        std::unique_ptr<build::PhemDict> dict = build::PhemDict::loadFromFiles(dictIs, forwardIs, backwardIs);
        
        std::set<utils::UniString> prefDict = build::loadPrefixDict(prefDictIs);
        classifier = utils::make_unique<ml::PhemCatBoostClassifier>(std::move(dict), prefDict, libPath, modelpath);
        corrector = std::make_unique<PhemCorrector>(prefDict);
    }
    void phemise(std::vector<analyze::WordFormPtr>& seq) const;
};
}
#endif
