#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <xmorphy/DAWG/Dictionary.h>
#include <boost/bimap.hpp>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/utils/UniString.h>

namespace X
{

struct LexemeGroup
{
    UniString prefix;
    UniSPTag sp;
    UniMorphTag tag;
    UniString suffix;
    bool isNormalForm;

    bool operator<(const LexemeGroup & o) const {
        return std::tie(prefix, sp, tag, suffix, isNormalForm) < std::tie(o.prefix, o.sp, o.tag, o.suffix, o.isNormalForm);
    }
};

struct EncodedLexemeGroup
{
    size_t prefixId;
    size_t tagId;
    size_t suffixId;
    bool isNormalForm;

    bool operator<(const EncodedLexemeGroup & o) const
    {
        return std::tie(prefixId, tagId, suffixId, isNormalForm) < std::tie(o.prefixId, o.tagId, o.suffixId, isNormalForm);
    }
};

struct AffixPair
{
    UniString prefix;
    UniString suffix;
};

using Paradigm = std::vector<LexemeGroup>;
using EncodedParadigm = std::vector<EncodedLexemeGroup>;

struct MorphTagPair
{
    UniSPTag sp;
    UniMorphTag tag;

    bool operator<(const MorphTagPair & o) const { return std::tie(sp, tag) < std::tie(o.sp, o.tag); }
};

struct ParaPair
{
    std::size_t paraNum;
    std::size_t formNum;
    std::size_t freq;
    bool operator==(const ParaPair & o) const { return paraNum == o.paraNum && formNum == o.formNum && freq == o.freq; }
};

struct ParaPairArray : public dawg::ISerializable
{
    std::vector<ParaPair> data;
    bool operator==(const ParaPairArray & o) const { return data == o.data; }
    bool serialize(std::ostream & os) const override
    {
        std::size_t sz = data.size();
        if (!os.write((const char *)(&sz), sizeof(std::size_t)))
            return false;
        if (!os.write((const char *)(&data[0]), sz * sizeof(ParaPair)))
            return false;
        return true;
    }
    bool deserialize(std::istream & is) override
    {
        std::size_t sz;
        if (!is.read((char *)(&sz), sizeof(std::size_t)))
            return false;
        data.resize(sz);
        if (!is.read((char *)(&data[0]), sizeof(ParaPair) * sz))
            return false;
        return true;
    }
};

struct PhemMarkup : public dawg::ISerializable
{
    using RawPhemTags = std::vector<uint8_t>;
    std::unordered_map<size_t, RawPhemTags> phem_map;

    bool operator==(const PhemMarkup & o) const { return phem_map == o.phem_map; }

    bool serialize(std::ostream & os) const override
    {
        size_t sz = phem_map.size();
        if (!os.write(reinterpret_cast<const char *>(&sz), sizeof(size_t)))
            return false;

        for (const auto & [key, data] : phem_map)
        {
            if (!os.write(reinterpret_cast<const char *>(&key), sizeof(size_t)))
                return false;

            size_t data_size = data.size();
            if (!os.write(reinterpret_cast<const char *>(&data_size), sizeof(size_t)))
                return false;

            if (!os.write(reinterpret_cast<const char *>(&data[0]), data_size * sizeof(uint8_t)))
                return false;
        }

        return true;
    }

    bool deserialize(std::istream & is) override
    {
        size_t map_size;
        if (!is.read(reinterpret_cast<char *>(&map_size), sizeof(size_t)))
            return false;

        while (phem_map.size() < map_size)
        {
            size_t key;
            if (!is.read(reinterpret_cast<char *>(&key), sizeof(size_t)))
                return false;

            size_t data_size;
            if (!is.read(reinterpret_cast<char *>(&data_size), sizeof(size_t)))
                return false;


            RawPhemTags data;
            data.resize(data_size);
            if (!is.read(reinterpret_cast<char *>(&data[0]), sizeof(uint8_t) * data_size))
                return false;

            phem_map[key] = data;
        }

        return true;
    }
};

const char DISAMBIG_SEPARATOR = '\xFF';
using DictPtr = std::shared_ptr<dawg::Dictionary<ParaPairArray>>;
using DisambDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;
using PhemDictPtr = std::shared_ptr<dawg::Dictionary<PhemMarkup>>;

using LoadFunc
= std::function<void(std::map<std::string, ParaPairArray> &, const std::vector<UniString> &, const std::vector<MorphTagPair> &, const std::vector<bool> & nf_mask)>;

using FilterFunc = std::function<void(std::map<std::string, ParaPairArray> &)>;

void saveParas(const std::vector<EncodedParadigm> & paraMap, std::ostream & os);
void loadParas(std::vector<EncodedParadigm> & paraMap, std::istream & is);
}
namespace std
{
template <>
struct hash<X::ParaPair>
{
public:
    size_t operator()(const X::ParaPair & s) const
    {
        size_t h1 = std::hash<std::size_t>()(s.paraNum);
        size_t h2 = std::hash<std::size_t>()(s.formNum);
        size_t h3 = std::hash<std::size_t>()(s.freq);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
template <>
struct hash<X::ParaPairArray>
{
public:
    size_t operator()(const X::ParaPairArray & s) const
    {
        size_t hashSum = 0;
        for (size_t i = 0; i < s.data.size(); ++i)
        {
            hashSum += std::hash<X::ParaPair>()(s.data[i]);
        }
        return hashSum;
    }
};
template <>
struct hash<X::PhemMarkup>
{
public:
    size_t operator()(const X::PhemMarkup & s) const
    {
        size_t hashSum = 0;
        for (const auto & [key, data] : s.phem_map)
        {
            hashSum += std::hash<size_t>()(key);
            for (size_t i = 0; i < data.size(); ++i)
            {
                hashSum += std::hash<uint8_t>()(data[i]);
            }
        }
        return hashSum;
    }
};
}
