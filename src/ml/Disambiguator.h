#pragma once
#include <ml/SpeechPartClassifier.h>
#include <ml/Embedding.h>

namespace ml
{

class Disambiguator
{
private:
    Embedding embedding;
    SpeechPartClassifier sp_classifier;
    size_t sequence_size;

public:
    Disambiguator(std::istream & embedding_, std::istream & speech_part_model_, size_t sequence_size_)
        : embedding(embedding_)
        , sp_classifier(speech_part_model_)
        , sequence_size(sequence_size_)
    {
    }

    std::vector<analyze::MorphInfo> disambiguate(const analyze::Sentence & forms) const;
};

}
