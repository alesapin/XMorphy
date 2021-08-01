#include <xmorphy/ml/TensorflowModel.h>
#include <tensorflow/lite/optional_debug_tools.h>
#include <iostream>

namespace X
{

#define TFLITE_MINIMAL_CHECK(x)                              \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

TensorflowModel::TensorflowModel(
    const char * caller_owned_buffer,
    size_t buffer_size)
    : model(tflite::FlatBufferModel::BuildFromBuffer(caller_owned_buffer, buffer_size))
{
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);
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

std::vector<float> TensorflowModel::predict(std::vector<float> && data) const
{
    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);

    std::cerr << "INPUTS SIZE:" << interpreter->inputs().size() << std::endl;

    size_t total_input_size = 1;
    for (const auto input : interpreter->inputs())
    {
        std::cerr << "INPUT:" << input << std::endl;
        std::cerr << "INPUT TENSOR:" << interpreter->tensor(input)->name << std::endl;
        auto * dims = interpreter->tensor(input)->dims;
        for (size_t i = 0; i < dims->size; ++i)
        {
            total_input_size *= dims->data[i];
            std::cerr << "INPUT TENSOR DIM:" << dims->data[i] << std::endl;
        }

        float * input_typed = interpreter->typed_input_tensor<float>(0);
        input_typed = &data[0];
        std::memcpy(input_typed, &data[0], data.size());
        std::cerr << "INPUTS:" << std::endl;
        for (size_t i = 0; i < total_input_size; ++i)
        {
            if (i != 0 && i % dims->data[2] == 0)
                std::cerr << '\n';
            std::cerr << input_typed[i] << ' ';

        }
    }

    std::cerr << "DATA SIZE:" << data.size() << std::endl;


    interpreter->Invoke();
    std::cerr << "OUTPUTS SIZE:" << interpreter->outputs().size() << std::endl;
    for (const auto output : interpreter->outputs())
    {
        std::cerr << "OUTPUT:" << output << std::endl;
        std::cerr << "OUTPUT TENSOR:" << interpreter->tensor(output)->name << std::endl;
        auto * dims = interpreter->tensor(output)->dims;
        for (size_t i = 0; i < dims->size; ++i)
        {
            std::cerr << "OUTPUT TENSOR DIM:" << dims->data[i] << std::endl;
        }
    }
    auto * dims = interpreter->tensor(interpreter->outputs()[0])->dims;
    size_t total_size = 1;
    for (size_t i = 0; i < dims->size; ++i)
    {
        total_size *= dims->data[i];
    }
    float * output = interpreter->typed_output_tensor<float>(0);
    std::cerr << "TOTAL SIZE:" << total_size <<std::endl;
    for (size_t i = 0; i < total_size; ++i)
    {
        if (i != 0 && i % dims->data[2] == 0)
        {
            std::cerr << "\n ARGMAX:" << std::max_element(&output[i - dims->data[2]], &output[i]) - &output[i - dims->data[2]] << std::endl;
            std::cerr << '\n';
        }
        std::cerr << output[i] << ' ';

    }
    std::cerr << std::endl;

    return std::vector<float>(output, output + getOutputShape(0).rows);
}

}
