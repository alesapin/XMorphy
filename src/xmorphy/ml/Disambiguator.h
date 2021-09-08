#pragma once
#include <xmorphy/ml/Embedding.h>
#include <xmorphy/ml/KerasModel.h>
#include <xmorphy/ml/KerasMultiModel.h>
#include <xmorphy/utils/LRUCache.h>
#include <xmorphy/ml/TensorflowModel.h>


namespace X
{
class Disambiguator
{
private:
    std::unique_ptr<Embedding> embedding;
    std::unique_ptr<KerasMultiModel> model;

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

    void processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const;

    std::vector<Sentence> splitSentenceToBatches(const Sentence & input) const;

    std::vector<MorphInfo> disambiguateImpl(const Sentence & forms, size_t sequence_size) const;

    Sentence filterTokens(const Sentence & input, std::vector<bool> & mask) const;

    size_t countIntersection(UniMorphTag target, UniMorphTag candidate) const;

public:
    Disambiguator();

    void disambiguate(Sentence & forms) const;
};

}
