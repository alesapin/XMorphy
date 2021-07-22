#include <xmorphy/ml/KerasMultiModel.h>

namespace X
{

fdeep::tensors KerasMultiModel::predict(size_t sequence_size, std::vector<float> && features) const
{
    auto model_it = predictors.lower_bound(sequence_size);
    if (model_it == predictors.end())
        throw std::runtime_error("Required to process sequence " + std::to_string(sequence_size) + " max " + std::to_string(predictors.rbegin()->first));
    return model_it->second->predict(std::move(features));
}

}
