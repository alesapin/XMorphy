#pragma once
#include <fdeep/fdeep.hpp>

namespace ml
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

    std::string getName() const
    {
        return model.name();
    }

    Shape getInputShape() const;
    Shape getOutputShape() const;

    std::vector<float> predictSingle(std::vector<float> && data);
};
}
