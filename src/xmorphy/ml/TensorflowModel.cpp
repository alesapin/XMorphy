#include <xmorphy/ml/TensorflowModel.h>
#include <tensorflow/lite/optional_debug_tools.h>
#include <iostream>

namespace X
{

namespace
{
    NonOwningTensor2d<float> convertToTensor2d(float * tensor, TfLiteIntArray * dims)
    {
        return NonOwningTensor2d<float>(tensor, dims->data[1], dims->data[2]);
    }

    ModelResult convertToTensors2dFrom3d(float * tensor, TfLiteIntArray * dims)
    {
        size_t total_result = dims->data[1];
        size_t width = dims->data[2];
        size_t height = dims->data[3];
        size_t one_tensor_length = width * height;
        ModelResult result;
        for (size_t i = 0; i < total_result; ++i)
            result.push_back(NonOwningTensor2d<float>(&tensor[i * one_tensor_length], width, height));

        return result;
    }
}

TensorflowModel::TensorflowModel(
    const char * caller_owned_buffer,
    size_t buffer_size)
    : model(tflite::FlatBufferModel::BuildFromBuffer(caller_owned_buffer, buffer_size))
{
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
    if (interpreter->AllocateTensors() != kTfLiteOk)
    {
        throw std::runtime_error("Cannot allocate tensors in tensorflow");
    }
}

Shape TensorflowModel::getInputShape(size_t dimension) const
{
    int input = interpreter->inputs()[dimension];
    TfLiteIntArray * dims = interpreter->tensor(input)->dims;
    size_t wanted_height = dims->data[1];
    size_t wanted_width = dims->data[2];
    size_t wanted_channels = dims->data[3];
    return {wanted_width, wanted_channels, wanted_height};
}

Shape TensorflowModel::getOutputShape(size_t dimension) const
{
    int output = interpreter->outputs()[dimension];
    TfLiteIntArray * dims = interpreter->tensor(output)->dims;
    size_t output_height = dims->data[1];
    size_t output_width = dims->data[2];
    size_t output_channels = dims->data[3];
    return {output_width, output_channels, output_height};
}

ModelResult TensorflowModel::predict(std::vector<float> && data) const
{
    float * input_typed = interpreter->typed_tensor<float>(interpreter->inputs()[0]);
    std::memcpy(input_typed, &data[0], sizeof(float) * data.size());

    //size_t total_input_size = 1;
    //for (const auto input : interpreter->inputs())
    //{
    //    std::cerr << "INPUT:" << input << std::endl;
    //    std::cerr << "INPUT TENSOR:" << interpreter->tensor(input)->name << std::endl;
    //    auto * dims = interpreter->tensor(input)->dims;
    //    for (size_t i = 0; i < dims->size; ++i)
    //    {
    //        total_input_size *= dims->data[i];
    //        //std::cerr << "INPUT TENSOR DIM:" << dims->data[i] << std::endl;
    //    }

    //    std::cerr << "TOTAL INPUT SIZE:" << total_input_size << " DataSIZE:" << data.size() << std::endl;

    //    std::cerr << "INPUTS:" << std::endl;
    //    for (size_t i = 0; i < total_input_size; ++i)
    //    {
    //        if (i != 0 && i % dims->data[2] == 0)
    //            std::cerr << '\n';
    //        std::cerr << input_typed[i] << ' ';
    //    }
    //}

    //std::cerr << "DATA SIZE:" << data.size() << std::endl;

    if (interpreter->Invoke() != kTfLiteOk)
    {
        throw std::runtime_error("Cannot invoke tensorflow interpreter");
    }

    //interpreter->Invoke();
    //std::cerr << "OUTPUTS SIZE:" << interpreter->outputs().size() << std::endl;
    //for (const auto output : interpreter->outputs())
    //{
    //    std::cerr << "OUTPUT:" << output << std::endl;
    //    std::cerr << "OUTPUT TENSOR:" << interpreter->tensor(output)->name << std::endl;
    //    auto * dims = interpreter->tensor(output)->dims;
    //    for (size_t i = 0; i < dims->size; ++i)
    //    {
    //        std::cerr << "OUTPUT TENSOR DIM:" << dims->data[i] << std::endl;
    //    }
    //}
    //auto * dims = interpreter->tensor(interpreter->outputs()[0])->dims;
    //size_t total_size = 1;
    //for (size_t i = 0; i < dims->size; ++i)
    //{
    //    total_size *= dims->data[i];
    //}
    ModelResult result;
    for (auto output_idx : interpreter->outputs())
    {
        float * output = interpreter->typed_tensor<float>(output_idx);
        auto * dims = interpreter->tensor(output_idx)->dims;
        auto tensor = convertToTensor2d(output, dims);
        result.push_back(tensor);
    }

    //float * output = interpreter->typed_output_tensor<float>(0);
    //std::cerr << "TOTAL SIZE:" << total_size <<std::endl;
    //for (size_t i = 0; i < total_size; ++i)
    //{
    //    if (i != 0 && i % dims->data[2] == 0)
    //    {
    //        std::cerr << "\n ARGMAX:" << std::max_element(&output[i - dims->data[2]], &output[i]) - &output[i - dims->data[2]] << std::endl;
    //        std::cerr << '\n';
    //    }
    //    std::cerr << output[i] << ' ';
    //}
    //std::cerr << std::endl;
    //std::cerr << "Output rows:" << getOutputShape(0).rows << std::endl;


    return result;
}

ModelResult TensorflowModel::predictTwoInputs(std::vector<float> && input1, std::vector<float> && input2) const
{
    //std::cerr << "INPUT SIZE:" << interpreter->inputs().size() << std::endl;
    //for (size_t i = 0; i < interpreter->inputs().size(); ++i)
    //{
    //    std::cerr << "INPUT:" << interpreter->inputs()[i] << std::endl;
    //    auto * dims = interpreter->tensor(interpreter->inputs()[i])->dims;
    //    std::cerr << "INPUT SHAPE\n";
    //    for (size_t j = 0; j < dims->size; ++j)
    //    {
    //        std::cerr << dims->data[j] << std::endl;
    //    }
    //}
    float * input1_typed = interpreter->typed_tensor<float>(interpreter->inputs()[0]);
    std::memcpy(input1_typed, &input1[0], sizeof(float) * input1.size());

    float * input2_typed = interpreter->typed_tensor<float>(interpreter->inputs()[1]);
    std::memcpy(input2_typed, &input2[0], sizeof(float) * input2.size());

    if (interpreter->Invoke() != kTfLiteOk)
    {
        throw std::runtime_error("Cannot invoke tensorflow interpreter");
    }

    ModelResult result;
    for (auto output_idx : interpreter->outputs())
    {
        //std::cerr << "OUTPUT:" << output_idx << std::endl;
        float * output = interpreter->typed_tensor<float>(output_idx);
        auto * dims = interpreter->tensor(output_idx)->dims;
        if (dims->size == 4)
        {
            auto tensors = convertToTensors2dFrom3d(output, dims);
            result.insert(result.end(), tensors.begin(), tensors.end());
        }
        else if (dims->size == 3)
        {
            auto tensor = convertToTensor2d(output, dims);
            result.push_back(tensor);
        }
        else
        {
            throw std::runtime_error("Cannot process tensor of such shape");
        }

        //for (size_t i = 0; i < dims->size; ++i)
        //{
        //    std::cerr << "DIM:[" << i << "] " << dims->data[i] << std::endl;
        //}
    }

    return result;
}

}
