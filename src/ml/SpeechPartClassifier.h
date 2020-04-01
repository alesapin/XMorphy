#pragma once
#include <ml/KerasModel.h>
#include <ml/Embedding.h>
#include <tag/UniSPTag.h>
#include <morph/WordForm.h>

namespace ml
{

class SpeechPartClassifier
{
private:
    KerasModel model;

public:
    SpeechPartClassifier(std::istream& is)
        : model(is)
    {
    }

    std::vector<float> getVectorWithPossibleParts(const analyze::WordFormPtr& wf) const;

    std::vector<base::UniSPTag> classifySpeechPart(std::vector<float> words) const;
};

}
