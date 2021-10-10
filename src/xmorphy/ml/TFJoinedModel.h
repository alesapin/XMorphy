#pragma once

#include <xmorphy/build/PhemDict.h>
#include <xmorphy/ml/Embedding.h>
#include <xmorphy/ml/KerasModel.h>
#include <xmorphy/ml/TensorflowModel.h>

#include <memory>

namespace X
{

class TFJoinedModel
{
private:
    std::unique_ptr<Embedding> embedding;
    std::unique_ptr<TensorflowMultiModel2d> model;
    std::unique_ptr<PhemDict> phem_dict;

    std::vector<Sentence> splitSentenceToBatches(const Sentence & input, size_t sentence_size) const;
    void fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillCaseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillNumberFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillGenderFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;
    void fillTenseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const;

    void getSpeechPartsFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const;
    void getCaseFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const;
    void getNumberFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const;
    void getGenderFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const;
    void getTenseFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const;

    Sentence filterTokens(const Sentence & input) const;

    bool disambiguateAndMorphemicSplitImpl(Sentence & forms) const;

    size_t countIntersection(UniMorphTag target, UniMorphTag candidate) const;

    void processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const;

    std::pair<size_t, size_t> selectModelForSentence(const Sentence & forms) const;
public:

    TFJoinedModel();

    bool disambiguateAndMorphemicSplit(Sentence & forms) const;
};

}
