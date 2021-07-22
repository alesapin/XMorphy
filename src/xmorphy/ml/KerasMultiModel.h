#pragma once
#include <xmorphy/ml/KerasModel.h>
#include <map>
#include <memory>

namespace X
{
using KerasModelPtr = std::shared_ptr<KerasModel>;
using KerasMultiModels = std::map<size_t, KerasModelPtr>;

class KerasMultiModel
{
private:
     KerasMultiModels predictors;
public:
    KerasMultiModel(KerasMultiModels && predictors_)
        : predictors(predictors_)
    {}

    fdeep::tensors predict(size_t sequence_size, std::vector<float> && features) const;

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

}
