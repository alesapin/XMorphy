#pragma once
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/tools/gen_op_registration.h>
#include <xmorphy/ml/KerasModel.h>

#include <vector>
#include <memory>

namespace X
{

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

    std::vector<float> predict(std::vector<float> && data) const;
};

}
