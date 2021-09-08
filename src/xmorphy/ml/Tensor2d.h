#pragma once
#include <algorithm>
#include <vector>
#include <iostream>

namespace X
{

template <typename T>
class NonOwningTensor2d
{
    T * data;

    size_t rows;
    size_t columns;

public:
    NonOwningTensor2d(T * data_, size_t rows_, size_t columns_)
        : data(data_)
        , rows(rows_)
        , columns(columns_)
    {
    }

    size_t getRows() const
    {
        return rows;
    }

    size_t getColumns() const
    {
        return columns;
    }

    T get(size_t row, size_t column) const
    {
        return data[row * columns + column];
    }

    size_t argmax(size_t row) const
    {
        auto row_start = &data[row * columns];
        auto row_end = &data[row * columns + columns];
        return std::max_element(row_start, row_end) - row_start;
    }

    void dumpTensor() const
    {
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < columns; ++j)
            {
                std::cerr << get(i, j) << ' ';
            }
            std::cerr << std::endl;
        }

    }
};

template <typename T>
using NonOwningTensors2d = std::vector<NonOwningTensor2d<T>>;

}
