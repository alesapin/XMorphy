#pragma once
#include <ml/Embedding.h>
#include <ml/KerasModel.h>

namespace ml
{

class Disambiguator
{
private:
    Embedding embedding;
    KerasModel model;
    size_t sequence_size;

    void fillSpeechPartFeature(const analyze::WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillCaseFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillNumberFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillGenderFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillTenseFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const;


    void getSpeechPartsFromTensor(const fdeep::tensor & tensor, std::vector<analyze::MorphInfo> & results) const;
    void getCaseFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const;
    void getNumberFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const;
    void getGenderFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const;
    void getTenseFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const;

public:
    Disambiguator(std::istream & embedding_, std::istream & model_stream_, size_t sequence_size_)
        : embedding(embedding_)
        , model(model_stream_)
        , sequence_size(sequence_size_)
    {
    }

    void disambiguate(analyze::Sentence & forms) const;
};

}
