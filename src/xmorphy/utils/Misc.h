#pragma once
#include <cmath>
#include <memory>
namespace utils
{
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
template <std::size_t SIZE>
void normalize(std::array<double, SIZE> & arr)
{
    double sum = 0;
    for (auto val : arr)
    {
        sum += val * val;
    }
    double squreRoot = std::sqrt(sum);
    for (auto & val : arr)
    {
        val = val / squreRoot;
    }
}
}
