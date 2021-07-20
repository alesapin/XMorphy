#pragma once

#include <xmorphy/build/PhemDict.h>
#include <xmorphy/ml/Embedding.h>
#include <xmorphy/ml/KerasModel.h>

#include <memory>

namespace X
{

class JoinedModel
{
private:
    std::unique_ptr<Embedding> embedding;
    std::unique_ptr<KerasModel> model;
    std::unique_ptr<PhemDict> phem_dict;

    size_t sentence_size;
    size_t word_size;


    std::vector<Sentence> splitSentenceToBatches(const Sentence & input) const;
    void fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillCaseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillNumberFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillGenderFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillTenseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;

    void getSpeechPartsFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;
    void getCaseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;
    void getNumberFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;
    void getGenderFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;
    void getTenseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const;

    Sentence filterTokens(const Sentence & input) const;

    void disambiguateAndMorphemicSplitImpl(Sentence & forms) const;

    size_t countIntersection(UniMorphTag target, UniMorphTag candidate) const;

    void processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const;
public:

    JoinedModel(std::istream & embedding_, std::istream & model_stream_, size_t sentence_size_, size_t word_size_)
        : embedding(std::make_unique<Embedding>(embedding_))
        , model(std::make_unique<KerasModel>(model_stream_))
        , sentence_size(sentence_size_)
        , word_size(word_size_)
    {

    }

    JoinedModel();

    void disambiguateAndMorphemicSplit(Sentence & forms) const;
};

}
