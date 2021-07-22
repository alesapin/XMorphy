#include <xmorphy/ml/KerasModel.h>

namespace X
{
KerasModel::KerasModel(std::istream & is) : model(fdeep::read_model(is, false, nullptr))
{
}

Shape KerasModel::getInputShape(size_t dimension) const
{
    auto input_shape = model.get_input_shapes()[dimension];
    return {input_shape.width_.unsafe_get_just(), input_shape.depth_.unsafe_get_just(), input_shape.height_.unsafe_get_just()};
}

Shape KerasModel::getOutputShape(size_t dimension) const
{
    auto output_shape = model.get_output_shapes()[dimension];
    return {output_shape.width_.unsafe_get_just(), output_shape.depth_.unsafe_get_just()};
}

fdeep::tensors KerasModel::predict(std::vector<float> && data) const
{
    auto [width, height, _] = getInputShape();
    return model.predict({fdeep::tensor(fdeep::tensor_shape(width, height), std::move(data))});
}

fdeep::tensors KerasModel::predictTwoInputs(std::vector<float> && data1, std::vector<float> && data2) const
{
    auto [first_width, first_height, first_depth] = getInputShape(0);
    auto [second_width, second_height, second_depth] = getInputShape(1);
    //std::cerr << "SECOND WIDTH:" << second_width << std::endl;
    //std::cerr << "SECOND HEIGTH:" << second_height<< std::endl;
    //std::cerr << "SECOND DEPTH:" << second_depth << std::endl;

    return model.predict({
        fdeep::tensor(fdeep::tensor_shape(first_width, first_height), std::move(data1)),
        fdeep::tensor(fdeep::tensor_shape(second_depth, second_width, second_height), std::move(data2))
    });
}

}
