#pragma once
#include <istream>
#include <string>
#include <vector>
#include <fdeep/fdeep.hpp>

namespace X
{

struct Shape
{
    size_t rows;
    size_t cols;
    size_t depth;
};


class KerasModel
{
private:
    fdeep::model model;

public:
    KerasModel(std::istream & is);

    std::string getName() const { return model.name(); }

    Shape getInputShape(size_t dimension = 0) const;
    Shape getOutputShape(size_t dimension = 0) const;

    fdeep::tensors predict(std::vector<float> && data) const;
    fdeep::tensors predictTwoInputs(std::vector<float> && data1, std::vector<float> && data2) const;
};
}
