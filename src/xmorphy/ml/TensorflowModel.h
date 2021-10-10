#pragma once
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <xmorphy/ml/KerasModel.h>
#include <xmorphy/ml/Tensor2d.h>

#include <vector>
#include <memory>

namespace X
{

using ModelResult = NonOwningTensors2d<float>;

class TensorflowModel
{
private:
    std::unique_ptr<tflite::FlatBufferModel> model;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;

public:
    TensorflowModel(
        const char * caller_owned_buffer,
        size_t buffer_size);

    Shape getInputShape(size_t dimension = 0) const;
    Shape getOutputShape(size_t dimension = 0) const;

    ModelResult predict(std::vector<float> && data) const;

    ModelResult predictTwoInputs(std::vector<float> && input1, std::vector<float> && input2) const;
};

using TensorflowModelPtr = std::shared_ptr<TensorflowModel>;
using TensorflowModels = std::map<size_t, TensorflowModelPtr>;

class TensorflowMultiModel
{
    TensorflowModels predictors;

public:
    TensorflowMultiModel(TensorflowModels && predictors_)
        : predictors(predictors_)
    {}

    ModelResult predict(size_t sequence_size, std::vector<float> && features) const
    {
        auto model_it = predictors.lower_bound(sequence_size);
        if (model_it == predictors.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(sequence_size) + " max " + std::to_string(predictors.rbegin()->first));
        return model_it->second->predict(std::move(features));
    }

    size_t getModelMaxSize() const
    {
        return predictors.rbegin()->first;
    }

    size_t roundSequenceSize(size_t sequence_size) const
    {
        auto model_it = predictors.lower_bound(sequence_size);
        if (model_it == predictors.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(sequence_size) + " max " + std::to_string(predictors.rbegin()->first));
        return model_it->first;
    }

};


using TensorflowModels2d = std::map<size_t, TensorflowModels>;

class TensorflowMultiModel2d
{
    TensorflowModels2d predictors;
public:
    TensorflowMultiModel2d(TensorflowModels2d && predictors_)
        : predictors(predictors_)
    {}


    size_t getModelMinSizeFirst() const
    {
        return predictors.begin()->first;
    }

    size_t getModelMaxSizeFirst() const
    {
        return predictors.rbegin()->first;
    }

    size_t getModelMaxSizeSecond() const
    {
        size_t max_size = 0;
        for (const auto & [_, inner] : predictors)
            max_size = std::max(inner.rbegin()->first, max_size);

        return max_size;
    }

    size_t getModelMaxSizeSecond(size_t first_size) const
    {
        return predictors.at(first_size).rbegin()->first;
    }

    size_t getRoundedSizeFirst(size_t first_size) const
    {
        auto inner_it = predictors.lower_bound(first_size);
        if (inner_it == predictors.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(first_size) + " max " + std::to_string(getModelMaxSizeFirst()));

        return inner_it->first;
    }


    std::pair<size_t, size_t> getRoundedSize(size_t first_size, size_t second_size) const
    {
        auto inner_it = predictors.lower_bound(first_size);
        if (inner_it == predictors.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(first_size) + " max " + std::to_string(getModelMaxSizeFirst()));

        auto model_it = inner_it->second.lower_bound(second_size);
        if (model_it == inner_it->second.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(second_size) + " max " + std::to_string(getModelMaxSizeSecond()));

        return std::make_pair(inner_it->first, model_it->first);
    }

    ModelResult predict(size_t first_size, size_t second_size, std::vector<float> && features1, std::vector<float> && features2) const
    {
        auto inner_it = predictors.lower_bound(first_size);
        if (inner_it == predictors.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(first_size) + " max " + std::to_string(getModelMaxSizeFirst()));

        auto model_it = inner_it->second.lower_bound(second_size);
        if (model_it == inner_it->second.end())
            throw std::runtime_error("Required to process sequence " + std::to_string(second_size) + " max " + std::to_string(getModelMaxSizeSecond()));

        return model_it->second->predictTwoInputs(std::move(features1), std::move(features2));
    }
};


}
