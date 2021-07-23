#pragma once
#include <bitset>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/bimap.hpp>
#include <boost/multiprecision/cpp_int.hpp>
namespace X
{
using namespace boost::multiprecision;
class ITag
{
    friend struct std::hash<ITag>;

protected:
    const boost::bimap<uint64_t, std::string> * name_map;
    uint64_t value;
    ITag(uint64_t val, const boost::bimap<uint64_t, std::string> * name_map) : name_map(name_map), value(val) {}
    ITag(const std::string & val, const boost::bimap<uint64_t, std::string> * name_map) : name_map(name_map), value(name_map->right.at(val))
    {
    }

public:
    explicit operator uint64_t() const { return value; }
    friend std::ostream & operator<<(std::ostream & os, const ITag & t) { return os << to_string(t); }
    friend std::istream & operator>>(std::istream & is, ITag & t)
    {
        std::string row;
        is >> row;
        from_string(row, t);
        return is;
    }
    ITag(const ITag & other)
    {
        value = other.value;
        name_map = other.name_map;
    }
    uint64_t getValue() const { return value; }
    friend std::string to_string(const ITag & t);
    friend std::string to_raw_string(const ITag & t);
    friend void from_raw_string(const std::string & str, ITag & t);
    friend void from_raw_string(const std::string_view & str, ITag & t);
    friend void from_string(const std::string & s, ITag & t);
    friend std::size_t count_intersection(const ITag & first, const ITag & second);
    virtual ITag operator|(const ITag & other) const;
    virtual bool operator&(const ITag & other) const;
    virtual ITag operator~() const;
    virtual void resetBits(const ITag & mask);
    virtual bool resetIfContains(const ITag & mask);
    virtual bool contains(const ITag & other) const;
    virtual ITag & operator|=(const ITag & other);
    virtual std::bitset<64> toBitset() const;
    std::string toString() const { return to_string(*this); }
    bool operator==(const ITag & other) const { return value == other.value && name_map == other.name_map; }
    bool operator!=(const ITag & other) const { return !this->operator==(other); }
    bool operator<(const ITag & other) const { return value < other.value; }
    bool operator>(const ITag & other) const { return value > other.value; }

    virtual ITag & operator=(const ITag & other)
    {
        value = other.value;
        name_map = other.name_map;
        return *this;
    }
    virtual ~ITag() {}
};

}
namespace std
{
template <>
struct hash<X::ITag>
{
    size_t operator()(X::ITag const & s) const
    {
        size_t h1{}, h2{}, r{};
        h1 = std::hash<uint64_t>{}(s.value);
        h2 = std::hash<void *>{}((void *)(s.name_map));
        boost::hash_combine(r, h1);
        boost::hash_combine(r, h2);
        return r;
    }
};
}
