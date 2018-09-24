#ifndef MORPH_TAG_H
#define MORPH_TAG_H
#include "ITag.h"
namespace base {

struct MorphTag : public ITag {
private:
    MorphTag(uint128_t val);
    static const std::vector<MorphTag> inner_runner;

public:
    static const MorphTag UNKN;
    static const MorphTag ANim;
    static const MorphTag anim;
    static const MorphTag inan;
    static const MorphTag GNdr;
    static const MorphTag masc;
    static const MorphTag femn;
    static const MorphTag neut;
    static const MorphTag Ms_f;
    static const MorphTag NMbr;
    static const MorphTag sing;
    static const MorphTag plur;
    static const MorphTag Sgtm;
    static const MorphTag Pltm;
    static const MorphTag Fixd;
    static const MorphTag CAse;
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
    static const MorphTag Abbr;
    static const MorphTag Name;
    static const MorphTag Surn;
    static const MorphTag Patr;
    static const MorphTag Geox;
    static const MorphTag Orgn;
    static const MorphTag Trad;
    static const MorphTag Subx;
    static const MorphTag Supr;
    static const MorphTag Qual;
    static const MorphTag Apro;
    static const MorphTag Anum;
    static const MorphTag Poss;
    static const MorphTag V_ey;
    static const MorphTag V_oy;
    static const MorphTag Cmp2;
    static const MorphTag V_ej;
    static const MorphTag ASpc;
    static const MorphTag perf;
    static const MorphTag impf;
    static const MorphTag TRns;
    static const MorphTag tran;
    static const MorphTag intr;
    static const MorphTag Impe;
    static const MorphTag Impx;
    static const MorphTag Mult;
    static const MorphTag Refl;
    static const MorphTag PErs;
    static const MorphTag per1;
    static const MorphTag per2;
    static const MorphTag per3;
    static const MorphTag TEns;
    static const MorphTag pres;
    static const MorphTag past;
    static const MorphTag futr;
    static const MorphTag MOod;
    static const MorphTag indc;
    static const MorphTag impr;
    static const MorphTag INvl;
    static const MorphTag incl;
    static const MorphTag excl;
    static const MorphTag VOic;
    static const MorphTag actv;
    static const MorphTag pssv;
    static const MorphTag Infr;
    static const MorphTag Slng;
    static const MorphTag Arch;
    static const MorphTag Litr;
    static const MorphTag Erro;
    static const MorphTag Dist;
    static const MorphTag Ques;
    static const MorphTag Dmns;
    static const MorphTag Prnt;
    static const MorphTag V_be;
    static const MorphTag V_en;
    static const MorphTag V_ie;
    static const MorphTag V_bi;
    static const MorphTag Fimp;
    static const MorphTag Prdx;
    static const MorphTag Coun;
    static const MorphTag Coll;
    static const MorphTag V_sh;
    static const MorphTag Af_p;
    static const MorphTag Inmx;
    static const MorphTag Vpre;
    static const MorphTag Anph;
    static const MorphTag Init;
    static const MorphTag Adjx;
    static std::vector<MorphTag>::const_iterator begin() {
        return inner_runner.begin();
    }
    static std::vector<MorphTag>::const_iterator end() {
        return inner_runner.end();
    }
    static std::size_t size() {
        return inner_runner.size();
    }

    MorphTag(const std::string& val);
    MorphTag();
    static constexpr std::size_t genderSize() {
        return 3;
    }
    static constexpr std::size_t numberSize() {
        return 2;
    }
    static constexpr std::size_t caseSize() {
        return 12;
    }
    MorphTag operator|(const MorphTag& o) const;
    MorphTag getCase() const;
    MorphTag getGender() const;
    MorphTag getNumber() const;
    MorphTag getTense() const;

    void setGender(const MorphTag &gender);
    void setNumber(const MorphTag &number);
    void setCase(const MorphTag &cas);

    MorphTag intersect(const ITag& o) const {
        uint128_t newValue = this->value & (uint128_t)o;
        return MorphTag(newValue);
    }


    static MorphTag getGen(std::size_t index) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::masc) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getGen(MorphTag t) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::masc) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }
    static MorphTag getNum(std::size_t index) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::sing) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getNum(MorphTag t) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::sing) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }
    static MorphTag getCase(std::size_t index) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::nomn) - inner_runner.begin();
        return inner_runner[start + index];
    }
    static std::size_t getCase(MorphTag t) {
        std::size_t start = std::find(inner_runner.begin(), inner_runner.end(), MorphTag::nomn) - inner_runner.begin();
        std::size_t result = (std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin()) - start;
        return result;
    }

    virtual ~MorphTag() {}

};
}
#endif
