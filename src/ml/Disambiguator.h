#pragma once
#include <ml/Embedding.h>
#include <ml/KerasModel.h>
#include <incbin.h>


namespace X
{

class Disambiguator
{
private:
    std::unique_ptr<Embedding> embedding;
    std::unique_ptr<KerasModel> model;
    size_t sequence_size;

    void fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillCaseFeature(const WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillNumberFeature(const WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillGenderFeature(const WordFormPtr form, std::vector<float>& data, size_t start) const;
    void fillTenseFeature(const WordFormPtr form, std::vector<float>& data, size_t start) const;

    void getSpeechPartsFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;
    void getCaseFromTensor(const fdeep::tensor& tensor, std::vector<MorphInfo>& results) const;
    void getNumberFromTensor(const fdeep::tensor& tensor, std::vector<MorphInfo>& results) const;
    void getGenderFromTensor(const fdeep::tensor& tensor, std::vector<MorphInfo>& results) const;
    void getTenseFromTensor(const fdeep::tensor& tensor, std::vector<MorphInfo>& results) const;

    void processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const;

    std::vector<Sentence> splitSentenceToBatches(const Sentence & input) const;

    std::vector<MorphInfo> disambiguateImpl(Sentence & forms) const;

    Sentence filterTokens(const Sentence & input, std::vector<bool> & mask) const;

    size_t smartCountIntersection(UniMorphTag target, UniMorphTag candidate) const;

public:
    Disambiguator(std::istream& embedding_, std::istream& model_stream_, size_t sequence_size_)
        : embedding(std::make_unique<Embedding>(embedding_))
        , model(std::make_unique<KerasModel>(model_stream_))
        , sequence_size(sequence_size_) {
    }
    Disambiguator();

    void disambiguate(Sentence & forms) const;
};

}
