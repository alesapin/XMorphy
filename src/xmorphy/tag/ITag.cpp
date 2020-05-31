#include "ITag.h"
#include <charconv>
#include <exception>
#include <string>
#include <string_view>

namespace X
{
std::string to_string(const ITag & t)
{
    if (t.name_map == nullptr)
        throw std::runtime_error("Trying to print empty tag");
    if (t.name_map->left.find(t.value) != t.name_map->left.end())
        return t.name_map->left.at(t.value);
    std::vector<std::string> resArr;
    for (auto itr = t.name_map->begin(); itr != t.name_map->end(); ++itr)
    {
        if (itr->left & t.value)
        {
            resArr.push_back(itr->right);
        }
    }
    std::sort(resArr.begin(), resArr.end());
    return boost::join(resArr, "|");
}
ITag ITag::operator|(const ITag & other) const
{
    if (name_map != other.name_map)
    {
        throw std::runtime_error("Different types in operator| of tags");
    }
    return ITag(value | other.value, name_map);
}
bool ITag::operator&(const ITag & other) const
{
    if (other.value == 0 && this->value == 0)
    {
        return true; //It's logical &
    }
    return (bool)(other.value & value);
}

ITag ITag::operator~() const
{
    return ITag(~(this->value), this->name_map);
}

ITag & ITag::operator|=(const ITag & other)
{
    if (name_map != other.name_map)
    {
        throw std::runtime_error("Different types in operator|= of tags");
    }
    value |= other.value;
    return *this;
}
bool ITag::contains(const ITag & other) const
{
    return other.value == (this->value & other.value);
}

void from_string(const std::string & s, ITag & t)
{
    std::vector<std::string> bits;
    boost::split(bits, s, boost::is_any_of(", |"));
    for (const auto & bit : bits)
    {
        auto itr = t.name_map->right.find(bit);
        if (itr != t.name_map->right.end())
        {
            t.value |= itr->second;
        }
        else
        {
            throw "Undefined tag name: " + bit;
        }
    }
}

void ITag::resetBits(const ITag & mask)
{
    value &= (~mask.value);
}

bool ITag::resetIfContains(const ITag & mask)
{
    if (contains(mask))
    {
        resetBits(mask);
        return true;
    }
    return false;
}

std::string to_raw_string(const ITag & t)
{
    return std::to_string(t.value);
}

void from_raw_string(const std::string & str, ITag & t)
{
    t.value = std::stoul(str);
}

void from_raw_string(const std::string_view & str, ITag & t)
{
    std::from_chars(str.begin(), str.end(), t.value);
}

std::size_t count_intersection(const ITag & first, const ITag & second)
{
    uint64_t inter = first.value & second.value;
    std::size_t counter = 0;
    uint64_t one = 1;
    for (std::size_t i = 0; i <= 64; i++)
    {
        counter += static_cast<bool>(inter & (one << i));
    }
    return counter;
}

std::bitset<64> ITag::toBitset() const
{
    uint128_t one = 1;
    std::bitset<64> result;
    for (std::size_t i = 0; i < 64; ++i)
    {
        if (value & (one << i))
        {
            result.set(i + 1);
        }
    }
    return result;
}

}
