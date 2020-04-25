#pragma once
#include <ml/Embedding.h>
#include <ml/KerasModel.h>
#include <incbin.h>


namespace ml
{

class Disambiguator
{
private:
    std::unique_ptr<Embedding> embedding;
    std::unique_ptr<KerasModel> model;
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

    void processFormsWithResultInfos(analyze::Sentence & forms, const std::vector<analyze::MorphInfo> & result_infos) const;

    std::vector<analyze::Sentence> splitSentenceToBatches(const analyze::Sentence & input) const;

    std::vector<analyze::MorphInfo> disambiguateImpl(analyze::Sentence & forms) const;

    analyze::Sentence filterTokens(const analyze::Sentence & input, std::vector<bool> & mask) const;

    size_t smartCountIntersection(base::UniMorphTag target, base::UniMorphTag candidate) const;

public:
    Disambiguator(std::istream& embedding_, std::istream& model_stream_, size_t sequence_size_)
        : embedding(std::make_unique<Embedding>(embedding_))
        , model(std::make_unique<KerasModel>(model_stream_))
        , sequence_size(sequence_size_) {
    }
    Disambiguator();

    void disambiguate(analyze::Sentence & forms) const;
};

}
