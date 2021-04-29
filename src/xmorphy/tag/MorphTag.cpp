#include "MorphTag.h"
namespace X
{
static const uint64_t ONE = 1;

static const boost::bimap<uint64_t, std::string> MORPH_TAG_MAP = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(
    0, "_")(ONE << 1, "anim")(ONE << 2, "inan")(ONE << 4, "masc")(ONE << 5, "femn")(ONE << 6, "neut")(ONE << 7, "Ms-f")(ONE << 9, "sing")(
    ONE << 10, "plur")(ONE << 11, "Sgtm")(ONE << 12, "Pltm")(ONE << 13, "Fixd")(ONE << 14, "nomn")(ONE << 15, "gent")(ONE << 16, "datv")(
    ONE << 17, "accs")(ONE << 18, "ablt")(ONE << 19, "loct")(ONE << 20, "voct")(ONE << 21, "gen1")(ONE << 22, "gen2")(ONE << 23, "acc2")(
    ONE << 24, "loc1")(ONE << 25, "loc2")(ONE << 26, "Subx")(ONE << 27, "Supr")(ONE << 28, "Qual")(ONE << 29, "Apro")(ONE << 30, "Anum")(
    ONE << 31, "Poss")(ONE << 32, "propn")(ONE << 33, "perf")(ONE << 34, "impf")(ONE << 35, "tran")(ONE << 36, "intr")(ONE << 37, "Impe")(
    ONE << 38, "Impx")(ONE << 39, "Mult")(ONE << 40, "Refl")(ONE << 41, "1per")(ONE << 42, "2per")(ONE << 43, "3per")(ONE << 44, "pres")(
    ONE << 45, "past")(ONE << 46, "futr")(ONE << 47, "indc")(ONE << 48, "impr")(ONE << 49, "incl")(ONE << 50, "excl")(ONE << 51, "actv")(
    ONE << 52, "pssv")(ONE << 53, "Vpre");

const MorphTag MorphTag::UNKN(uint64_t(0));
const MorphTag MorphTag::anim(ONE << 1);
const MorphTag MorphTag::inan(ONE << 2);
const MorphTag MorphTag::masc(ONE << 4);
const MorphTag MorphTag::femn(ONE << 5);
const MorphTag MorphTag::neut(ONE << 6);
const MorphTag MorphTag::Ms_f(ONE << 7);
const MorphTag MorphTag::sing(ONE << 9);
const MorphTag MorphTag::plur(ONE << 10);
const MorphTag MorphTag::Sgtm(ONE << 11);
const MorphTag MorphTag::Pltm(ONE << 12);
const MorphTag MorphTag::Fixd(ONE << 13);
const MorphTag MorphTag::nomn(ONE << 14);
const MorphTag MorphTag::gent(ONE << 15);
const MorphTag MorphTag::datv(ONE << 16);
const MorphTag MorphTag::accs(ONE << 17);
const MorphTag MorphTag::ablt(ONE << 18);
const MorphTag MorphTag::loct(ONE << 19);
const MorphTag MorphTag::voct(ONE << 20);
const MorphTag MorphTag::gen1(ONE << 21);
const MorphTag MorphTag::gen2(ONE << 22);
const MorphTag MorphTag::acc2(ONE << 23);
const MorphTag MorphTag::loc1(ONE << 24);
const MorphTag MorphTag::loc2(ONE << 25);
const MorphTag MorphTag::Subx(ONE << 26);
const MorphTag MorphTag::Supr(ONE << 27);
const MorphTag MorphTag::Qual(ONE << 28);
const MorphTag MorphTag::Apro(ONE << 29);
const MorphTag MorphTag::Anum(ONE << 30);
const MorphTag MorphTag::Poss(ONE << 31);
const MorphTag MorphTag::propn(ONE << 32);
const MorphTag MorphTag::perf(ONE << 33);
const MorphTag MorphTag::impf(ONE << 34);
const MorphTag MorphTag::tran(ONE << 35);
const MorphTag MorphTag::intr(ONE << 36);
const MorphTag MorphTag::Impe(ONE << 37);
const MorphTag MorphTag::Impx(ONE << 38);
const MorphTag MorphTag::Mult(ONE << 39);
const MorphTag MorphTag::Refl(ONE << 40);
const MorphTag MorphTag::per1(ONE << 41);
const MorphTag MorphTag::per2(ONE << 42);
const MorphTag MorphTag::per3(ONE << 43);
const MorphTag MorphTag::pres(ONE << 44);
const MorphTag MorphTag::past(ONE << 45);
const MorphTag MorphTag::futr(ONE << 46);
const MorphTag MorphTag::indc(ONE << 47);
const MorphTag MorphTag::impr(ONE << 48);
const MorphTag MorphTag::incl(ONE << 49);
const MorphTag MorphTag::excl(ONE << 50);
const MorphTag MorphTag::actv(ONE << 51);
const MorphTag MorphTag::pssv(ONE << 52);
const MorphTag MorphTag::Vpre(ONE << 53);

const std::vector<MorphTag> MorphTag::inner_runner
    = {UNKN, anim, inan, masc, femn, neut, Ms_f, sing, plur, Sgtm, Pltm, Fixd, nomn, gent, datv, accs, ablt, loct,
       voct, gen1, gen2, acc2, loc1, loc2, Subx, Supr, Qual, Apro, Anum, Poss, propn, perf, impf, tran, intr, Impe,
       Impx, Mult, Refl, per1, per2, per3, pres, past, futr, indc, impr, incl, excl, actv, pssv, Vpre};

static ITag CASE_MASK = MorphTag::nomn | MorphTag::gent | MorphTag::datv | MorphTag::accs | MorphTag::ablt | MorphTag::loct | MorphTag::voct
    | MorphTag::gen1 | MorphTag::gen2 | MorphTag::acc2 | MorphTag::loc1 | MorphTag::loc2;

static ITag GENDER_MASK = MorphTag::neut | MorphTag::femn | MorphTag::masc;
static ITag NUMBER_MASK = MorphTag::sing | MorphTag::plur;
static ITag TENSE_MASK = MorphTag::pres | MorphTag::futr | MorphTag::past;

MorphTag::MorphTag(uint64_t val) : ITag(val, &MORPH_TAG_MAP)
{
}
MorphTag::MorphTag(const std::string & val) : ITag(val, &MORPH_TAG_MAP)
{
}
MorphTag::MorphTag() : ITag((uint64_t)0, &MORPH_TAG_MAP)
{
}

MorphTag MorphTag::operator|(const MorphTag & o) const
{
    return MorphTag((uint64_t)this->ITag::operator|(o));
}

MorphTag MorphTag::getNumber() const
{
    return this->intersect(NUMBER_MASK);
}
MorphTag MorphTag::getGender() const
{
    return this->intersect(GENDER_MASK);
}
MorphTag MorphTag::getCase() const
{
    return this->intersect(CASE_MASK);
}
MorphTag MorphTag::getTense() const
{
    return this->intersect(TENSE_MASK);
}

void MorphTag::setGender(const MorphTag & gender)
{
    *this = intersect(~GENDER_MASK);
    *this |= gender;
}
void MorphTag::setNumber(const MorphTag & number)
{
    *this = intersect(~NUMBER_MASK);
    *this |= number;
}
void MorphTag::setCase(const MorphTag & cas)
{
    *this = intersect(~CASE_MASK);
    *this |= cas;
}
}
