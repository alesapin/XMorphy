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
};


class KerasModel
{
private:
    fdeep::model model;

public:
    KerasModel(std::istream & is);

    std::string getName() const { return model.name(); }

    Shape getInputShape() const;
    Shape getOutputShape() const;

    fdeep::tensors predict(std::vector<float> && data) const;
};
}
