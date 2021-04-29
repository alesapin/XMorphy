#pragma once
#include "ITag.h"
namespace X
{
struct MorphTag : public ITag
{
private:
    MorphTag(uint64_t val);
    static const std::vector<MorphTag> inner_runner;

public:
    static const MorphTag UNKN;
    static const MorphTag anim;
    static const MorphTag inan;
    static const MorphTag masc;
    static const MorphTag femn;
    static const MorphTag neut;
    static const MorphTag Ms_f;
    static const MorphTag sing;
    static const MorphTag plur;
    static const MorphTag Sgtm;
    static const MorphTag Pltm;
    static const MorphTag Fixd;
    static const MorphTag nomn;
    static const MorphTag gent;
    static const MorphTag datv;
    static const MorphTag accs;
    static const MorphTag ablt;
    static const MorphTag loct;
    static const MorphTag voct;
    static const MorphTag gen1;
    static const MorphTag gen2;
    static const MorphTag acc2;
    static const MorphTag loc1;
    static const MorphTag loc2;
    static const MorphTag Subx;
    static const MorphTag Supr;
    static const MorphTag Qual;
    static const MorphTag Apro;
    static const MorphTag Anum;
    static const MorphTag Poss;
    static const MorphTag propn;
    static const MorphTag perf;
    static const MorphTag impf;
    static const MorphTag tran;
    static const MorphTag intr;
    static const MorphTag Impe;
    static const MorphTag Impx;
    static const MorphTag Mult;
    static const MorphTag Refl;
    static const MorphTag per1;
    static const MorphTag per2;
    static const MorphTag per3;
    static const MorphTag pres;
    static const MorphTag past;
    static const MorphTag futr;
    static const MorphTag indc;
    static const MorphTag impr;
    static const MorphTag incl;
    static const MorphTag excl;
    static const MorphTag actv;
    static const MorphTag pssv;
    static const MorphTag Vpre;

    static std::vector<MorphTag>::const_iterator begin() { return inner_runner.begin(); }
    static std::vector<MorphTag>::const_iterator end() { return inner_runner.end(); }
    static std::size_t size() { return inner_runner.size(); }

    MorphTag(const std::string & val);
    MorphTag();
    static constexpr std::size_t genderSize() { return 3; }
    static constexpr std::size_t numberSize() { return 2; }
    static constexpr std::size_t caseSize() { return 12; }
    MorphTag operator|(const MorphTag & o) const;
    MorphTag getCase() const;
    MorphTag getGender() const;
    MorphTag getNumber() const;
    MorphTag getTense() const;

    void setGender(const MorphTag & gender);
    void setNumber(const MorphTag & number);
    void setCase(const MorphTag & cas);

    MorphTag intersect(const ITag & o) const
    {
        uint64_t newValue = this->value & (uint64_t)o;
        return MorphTag(newValue);
    }


    static MorphTag getGen(std::size_t index)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::masc) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getGen(MorphTag t)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::masc) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }
    static MorphTag getNum(std::size_t index)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::sing) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getNum(MorphTag t)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::sing) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }
    static MorphTag getCase(std::size_t index)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::nomn) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getCase(MorphTag t)
    {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::nomn) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }

    virtual ~MorphTag() {}
};
}
