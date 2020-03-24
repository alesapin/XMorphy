#include <ml/KerasModel.h>

namespace ml
{

KerasModel::KerasModel(std::istream& is)
    : model(fdeep::read_model(is))
{
}

Shape KerasModel::getInputShape() const
{
    auto input_shape = model.get_input_shapes()[0];
    return {input_shape.width_.unsafe_get_just(), input_shape.depth_.unsafe_get_just()};
}

Shape KerasModel::getOutputShape() const
{
    auto output_shape = model.get_output_shapes()[0];
    return {output_shape.width_.unsafe_get_just(), output_shape.depth_.unsafe_get_just()};
}

std::vector<float> KerasModel::predictSingle(std::vector<float> && data)
{
    auto [width, height] = getInputShape();
    auto tmp_res = model.predict({fdeep::tensor(fdeep::tensor_shape(width, height), std::move(data))});
    std::vector<float> result(tmp_res[0].as_vector()->begin(), tmp_res[0].as_vector()->end());
    return result;
}
}
