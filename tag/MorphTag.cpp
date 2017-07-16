#include "MorphTag.h"
namespace base {
static const uint128_t ONE = 1;

static const boost::bimap<uint128_t, std::string> MORPH_TAG_MAP =
    boost::assign::list_of<boost::bimap<uint128_t, std::string>::relation>(0, "_")(ONE << 0, "ANim")(ONE << 1, "anim")(ONE << 2, "inan")(ONE << 3, "GNdr")(ONE << 4, "masc")(ONE << 5, "femn")(ONE << 6, "neut")(ONE << 7, "Ms-f")(ONE << 8, "NMbr")(ONE << 9, "sing")(ONE << 10, "plur")(ONE << 11, "Sgtm")(ONE << 12, "Pltm")(ONE << 13, "Fixd")(ONE << 14, "CAse")(ONE << 15, "nomn")(ONE << 16, "gent")(ONE << 17, "datv")(ONE << 18, "accs")(ONE << 19, "ablt")(ONE << 20, "loct")(ONE << 21, "voct")(ONE << 22, "gen1")(ONE << 23, "gen2")(ONE << 24, "acc2")(ONE << 25, "loc1")(ONE << 26, "loc2")(ONE << 27, "Abbr")(ONE << 28, "Name")(ONE << 29, "Surn")(ONE << 30, "Patr")(ONE << 31, "Geox")(ONE << 32, "Orgn")(ONE << 33, "Trad")(ONE << 34, "Subx")(ONE << 35, "Supr")(ONE << 36, "Qual")(ONE << 37, "Apro")(ONE << 38, "Anum")(ONE << 39, "Poss")(ONE << 40, "V-ey")(ONE << 41, "V-oy")(ONE << 42, "Cmp2")(ONE << 43, "V-ej")(ONE << 44, "ASpc")(ONE << 45, "perf")(ONE << 46, "impf")(ONE << 47, "TRns")(ONE << 48, "tran")(ONE << 49, "intr")(ONE << 50, "Impe")(ONE << 51, "Impx")(ONE << 52, "Mult")(ONE << 53, "Refl")(ONE << 54, "PErs")(ONE << 55, "1per")(ONE << 56, "2per")(ONE << 57, "3per")(ONE << 58, "TEns")(ONE << 59, "pres")(ONE << 60, "past")(ONE << 61, "futr")(ONE << 62, "MOod")(ONE << 63, "indc")(ONE << 64, "impr")(ONE << 65, "INvl")(ONE << 66, "incl")(ONE << 67, "excl")(ONE << 68, "VOic")(ONE << 69, "actv")(ONE << 70, "pssv")(ONE << 71, "Infr")(ONE << 72, "Slng")(ONE << 73, "Arch")(ONE << 74, "Litr")(ONE << 75, "Erro")(ONE << 76, "Dist")(ONE << 77, "Ques")(ONE << 78, "Dmns")(ONE << 79, "Prnt")(ONE << 80, "V-be")(ONE << 81, "V-en")(ONE << 82, "V-ie")(ONE << 83, "V-bi")(ONE << 84, "Fimp")(ONE << 85, "Prdx")(ONE << 86, "Coun")(ONE << 87, "Coll")(ONE << 88, "V-sh")(ONE << 89, "Af-p")(ONE << 90, "Inmx")(ONE << 91, "Vpre")(ONE << 92, "Anph")(ONE << 93, "Init")(ONE << 94, "Adjx");
const MorphTag MorphTag::UNKN(uint128_t(0));
const MorphTag MorphTag::ANim(ONE << 0);
const MorphTag MorphTag::anim(ONE << 1);
const MorphTag MorphTag::inan(ONE << 2);
const MorphTag MorphTag::GNdr(ONE << 3);
const MorphTag MorphTag::masc(ONE << 4);
const MorphTag MorphTag::femn(ONE << 5);
const MorphTag MorphTag::neut(ONE << 6);
const MorphTag MorphTag::Ms_f(ONE << 7);
const MorphTag MorphTag::NMbr(ONE << 8);
const MorphTag MorphTag::sing(ONE << 9);
const MorphTag MorphTag::plur(ONE << 10);
const MorphTag MorphTag::Sgtm(ONE << 11);
const MorphTag MorphTag::Pltm(ONE << 12);
const MorphTag MorphTag::Fixd(ONE << 13);
const MorphTag MorphTag::CAse(ONE << 14);
const MorphTag MorphTag::nomn(ONE << 15);
const MorphTag MorphTag::gent(ONE << 16);
const MorphTag MorphTag::datv(ONE << 17);
const MorphTag MorphTag::accs(ONE << 18);
const MorphTag MorphTag::ablt(ONE << 19);
const MorphTag MorphTag::loct(ONE << 20);
const MorphTag MorphTag::voct(ONE << 21);
const MorphTag MorphTag::gen1(ONE << 22);
const MorphTag MorphTag::gen2(ONE << 23);
const MorphTag MorphTag::acc2(ONE << 24);
const MorphTag MorphTag::loc1(ONE << 25);
const MorphTag MorphTag::loc2(ONE << 26);
const MorphTag MorphTag::Abbr(ONE << 27);
const MorphTag MorphTag::Name(ONE << 28);
const MorphTag MorphTag::Surn(ONE << 29);
const MorphTag MorphTag::Patr(ONE << 30);
const MorphTag MorphTag::Geox(ONE << 31);
const MorphTag MorphTag::Orgn(ONE << 32);
const MorphTag MorphTag::Trad(ONE << 33);
const MorphTag MorphTag::Subx(ONE << 34);
const MorphTag MorphTag::Supr(ONE << 35);
const MorphTag MorphTag::Qual(ONE << 36);
const MorphTag MorphTag::Apro(ONE << 37);
const MorphTag MorphTag::Anum(ONE << 38);
const MorphTag MorphTag::Poss(ONE << 39);
const MorphTag MorphTag::V_ey(ONE << 40);
const MorphTag MorphTag::V_oy(ONE << 41);
const MorphTag MorphTag::Cmp2(ONE << 42);
const MorphTag MorphTag::V_ej(ONE << 43);
const MorphTag MorphTag::ASpc(ONE << 44);
const MorphTag MorphTag::perf(ONE << 45);
const MorphTag MorphTag::impf(ONE << 46);
const MorphTag MorphTag::TRns(ONE << 47);
const MorphTag MorphTag::tran(ONE << 48);
const MorphTag MorphTag::intr(ONE << 49);
const MorphTag MorphTag::Impe(ONE << 50);
const MorphTag MorphTag::Impx(ONE << 51);
const MorphTag MorphTag::Mult(ONE << 52);
const MorphTag MorphTag::Refl(ONE << 53);
const MorphTag MorphTag::PErs(ONE << 54);
const MorphTag MorphTag::per1(ONE << 55);
const MorphTag MorphTag::per2(ONE << 56);
const MorphTag MorphTag::per3(ONE << 57);
const MorphTag MorphTag::TEns(ONE << 58);
const MorphTag MorphTag::pres(ONE << 59);
const MorphTag MorphTag::past(ONE << 60);
const MorphTag MorphTag::futr(ONE << 61);
const MorphTag MorphTag::MOod(ONE << 62);
const MorphTag MorphTag::indc(ONE << 63);
const MorphTag MorphTag::impr(ONE << 64);
const MorphTag MorphTag::INvl(ONE << 65);
const MorphTag MorphTag::incl(ONE << 66);
const MorphTag MorphTag::excl(ONE << 67);
const MorphTag MorphTag::VOic(ONE << 68);
const MorphTag MorphTag::actv(ONE << 69);
const MorphTag MorphTag::pssv(ONE << 70);
const MorphTag MorphTag::Infr(ONE << 71);
const MorphTag MorphTag::Slng(ONE << 72);
const MorphTag MorphTag::Arch(ONE << 73);
const MorphTag MorphTag::Litr(ONE << 74);
const MorphTag MorphTag::Erro(ONE << 75);
const MorphTag MorphTag::Dist(ONE << 76);
const MorphTag MorphTag::Ques(ONE << 77);
const MorphTag MorphTag::Dmns(ONE << 78);
const MorphTag MorphTag::Prnt(ONE << 79);
const MorphTag MorphTag::V_be(ONE << 80);
const MorphTag MorphTag::V_en(ONE << 81);
const MorphTag MorphTag::V_ie(ONE << 82);
const MorphTag MorphTag::V_bi(ONE << 83);
const MorphTag MorphTag::Fimp(ONE << 84);
const MorphTag MorphTag::Prdx(ONE << 85);
const MorphTag MorphTag::Coun(ONE << 86);
const MorphTag MorphTag::Coll(ONE << 87);
const MorphTag MorphTag::V_sh(ONE << 88);
const MorphTag MorphTag::Af_p(ONE << 89);
const MorphTag MorphTag::Inmx(ONE << 90);
const MorphTag MorphTag::Vpre(ONE << 91);
const MorphTag MorphTag::Anph(ONE << 92);
const MorphTag MorphTag::Init(ONE << 93);
const MorphTag MorphTag::Adjx(ONE << 94);

const std::vector<MorphTag> MorphTag::inner_runner = {
    UNKN,
    ANim,
    anim,
    inan,
    GNdr,
    masc,
    femn,
    neut,
    Ms_f,
    NMbr,
    sing,
    plur,
    Sgtm,
    Pltm,
    Fixd,
    CAse,
    nomn,
    gent,
    datv,
    accs,
    ablt,
    loct,
    voct,
    gen1,
    gen2,
    acc2,
    loc1,
    loc2,
    Abbr,
    Name,
    Surn,
    Patr,
    Geox,
    Orgn,
    Trad,
    Subx,
    Supr,
    Qual,
    Apro,
    Anum,
    Poss,
    V_ey,
    V_oy,
    Cmp2,
    V_ej,
    ASpc,
    perf,
    impf,
    TRns,
    tran,
    intr,
    Impe,
    Impx,
    Mult,
    Refl,
    PErs,
    per1,
    per2,
    per3,
    TEns,
    pres,
    past,
    futr,
    MOod,
    indc,
    impr,
    INvl,
    incl,
    excl,
    VOic,
    actv,
    pssv,
    Infr,
    Slng,
    Arch,
    Litr,
    Erro,
    Dist,
    Ques,
    Dmns,
    Prnt,
    V_be,
    V_en,
    V_ie,
    V_bi,
    Fimp,
    Prdx,
    Coun,
    Coll,
    V_sh,
    Af_p,
    Inmx,
    Vpre,
    Anph,
    Init,
    Adjx,
};

static ITag CASE_MASK = MorphTag::nomn | MorphTag::gent | MorphTag::datv | MorphTag::accs | MorphTag::ablt | MorphTag::loct | MorphTag::voct | MorphTag::gen1 | MorphTag::gen2 | MorphTag::acc2 | MorphTag::loc1 | MorphTag::loc2;

static ITag GENDER_MASK = MorphTag::neut | MorphTag::femn | MorphTag::masc;
static ITag NUMBER_MASK = MorphTag::sing | MorphTag::plur;
static ITag TENSE_MASK = MorphTag::pres | MorphTag::futr | MorphTag::past;

MorphTag::MorphTag(uint128_t val)
    : ITag(val, &MORPH_TAG_MAP) {
}
MorphTag::MorphTag(const std::string& val)
    : ITag(val, &MORPH_TAG_MAP) {
}
MorphTag::MorphTag()
    : ITag((uint128_t)0, &MORPH_TAG_MAP) {
}

MorphTag MorphTag::operator|(const MorphTag& o) const {
    return MorphTag((uint128_t) this->ITag::operator|(o));
}

MorphTag MorphTag::getNumber() const {
    return this->intersect(NUMBER_MASK);
}
MorphTag MorphTag::getGender() const {
    return this->intersect(GENDER_MASK);
}
MorphTag MorphTag::getCase() const {
    return this->intersect(CASE_MASK);
}
MorphTag MorphTag::getTense() const {
    return this->intersect(TENSE_MASK);
}

void MorphTag::setGender(const MorphTag& gender) {
    *this = intersect(~GENDER_MASK);
    *this |= gender;
}
void MorphTag::setNumber(const MorphTag& number) {
    *this = intersect(~NUMBER_MASK);
    *this |= number;
}
void MorphTag::setCase(const MorphTag& cas) {
    *this = intersect(~CASE_MASK);
    *this |= cas;
}
}
