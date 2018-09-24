#ifndef _PHEMMER_H
#define _PHEMMER_H
#include <build/PhemDict.h>
#include <morph/WordForm.h>
#include <ml/PhemClassifier.h>
#include <vector>
#include <memory>

namespace phem {
class Phemmer {
    std::unique_ptr<ml::PhemClassifier> classifier;

public:
    Phemmer(const std::string& dictpath, const std::string& modelpath) {
        std::unique_ptr<build::PhemDict> dict;
        loadFromFiles(dict, dictpath);
        classifier = utils::make_unique<ml::PhemClassifier>(std::move(dict), modelpath);
    }
    void phemise(std::vector<analyze::WordFormPtr>& seq) const;
};
}
#endif
