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
    utils::UniString prefix;
    UniSPTag sp;
    UniMorphTag tag;
    utils::UniString suffix;
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
    utils::UniString prefix;
    utils::UniString suffix;
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
    std::vector<uint8_t> data;

    bool operator==(const PhemMarkup & o) const { return data == o.data; }

    PhemTag getTag(std::size_t index) const { return PhemTag::get(index); }
    void append(const PhemTag & tag) { data.push_back(PhemTag::get(tag)); }

    bool serialize(std::ostream & os) const override
    {
        std::size_t sz = data.size();
        if (!os.write((const char *)(&sz), sizeof(std::size_t)))
            return false;
        if (!os.write((const char *)(&data[0]), sz * sizeof(uint8_t)))
            return false;
        return true;
    }
    bool deserialize(std::istream & is) override
    {
        std::size_t sz;
        if (!is.read((char *)(&sz), sizeof(std::size_t)))
            return false;
        data.resize(sz);
        if (!is.read((char *)(&data[0]), sizeof(uint8_t) * sz))
            return false;
        return true;
    }
};

const char DISAMBIG_SEPARATOR = '\xFF';
using DictPtr = std::shared_ptr<dawg::Dictionary<ParaPairArray>>;
using DisambDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;
using InnerPhemDictPtr = std::shared_ptr<dawg::Dictionary<PhemMarkup>>;
using InnerCounterPhemDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;
using InnerCounterPhemDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;

using LoadFunc
= std::function<void(std::map<std::string, ParaPairArray> &, const std::vector<utils::UniString> &, const std::vector<MorphTagPair> &, const std::vector<bool> & nf_mask)>;

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
        for (size_t i = 0; i < s.data.size(); ++i)
        {
            hashSum += std::hash<uint8_t>()(s.data[i]);
        }
        return hashSum;
    }
};
}
