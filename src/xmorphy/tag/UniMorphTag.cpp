#include "UniMorphTag.h"
namespace X
{
static const uint64_t ONE = 1;
static const boost::bimap<uint64_t, std::string> UNI_MORPH_MAP = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(
    0x00, "_")(ONE << 0, "Gender=Masc")(ONE << 1, "Gender=Fem")(ONE << 2, "Gender=Neut")(ONE << 3, "Animacy=Anim")(
    ONE << 4, "Animacy=Inan")(ONE << 5, "Number=Sing")(ONE << 6, "Number=Plur")(ONE << 7, "Case=Ins")(ONE << 8, "Case=Acc")(
    ONE << 9, "Case=Nom")(ONE << 10, "Case=Dat")(ONE << 11, "Case=Gen")(ONE << 12, "Case=Loc")(ONE << 13, "Degree=Cmp")(
    ONE << 14, "Degree=Sup")(ONE << 15, "VerbForm=Fin")(ONE << 16, "VerbForm=Inf")(ONE << 17, "Case=Voc")(ONE << 19, "Mood=Imp")(
    ONE << 20, "Mood=Ind")(ONE << 21, "Person=1")(ONE << 22, "Person=2")(ONE << 23, "Person=3")(ONE << 24, "Tense=Fut")(
    ONE << 25, "Tense=Past")(ONE << 26, "Tense=Pres")(ONE << 27, "Variant=Short")(ONE << 28, "Voice=Act")(ONE << 29, "Voice=Pass")(
    ONE << 30, "Degree=Pos")(ONE << 31, "Tense=Notpast")(ONE << 32, "VerbForm=Conv")(ONE << 33, "Voice=Mid")(ONE << 34, "NumForm=Digit");

const UniMorphTag UniMorphTag::UNKN((uint64_t)0);
const UniMorphTag UniMorphTag::Masc(ONE << 0);
const UniMorphTag UniMorphTag::Fem(ONE << 1);
const UniMorphTag UniMorphTag::Neut(ONE << 2);
const UniMorphTag UniMorphTag::Anim(ONE << 3);
const UniMorphTag UniMorphTag::Inan(ONE << 4);
const UniMorphTag UniMorphTag::Sing(ONE << 5);
const UniMorphTag UniMorphTag::Plur(ONE << 6);
const UniMorphTag UniMorphTag::Ins(ONE << 7);
const UniMorphTag UniMorphTag::Acc(ONE << 8);
const UniMorphTag UniMorphTag::Nom(ONE << 9);
const UniMorphTag UniMorphTag::Dat(ONE << 10);
const UniMorphTag UniMorphTag::Gen(ONE << 11);
const UniMorphTag UniMorphTag::Loc(ONE << 12);
const UniMorphTag UniMorphTag::Cmp(ONE << 13);
const UniMorphTag UniMorphTag::Sup(ONE << 14);
const UniMorphTag UniMorphTag::Fin(ONE << 15);
const UniMorphTag UniMorphTag::Inf(ONE << 16);
const UniMorphTag UniMorphTag::Voc(ONE << 17);
const UniMorphTag UniMorphTag::Imp(ONE << 19);
const UniMorphTag UniMorphTag::Ind(ONE << 20);
const UniMorphTag UniMorphTag::_1(ONE << 21);
const UniMorphTag UniMorphTag::_2(ONE << 22);
const UniMorphTag UniMorphTag::_3(ONE << 23);
const UniMorphTag UniMorphTag::Fut(ONE << 24);
const UniMorphTag UniMorphTag::Past(ONE << 25);
const UniMorphTag UniMorphTag::Pres(ONE << 26);
const UniMorphTag UniMorphTag::Short(ONE << 27);
const UniMorphTag UniMorphTag::Act(ONE << 28);
const UniMorphTag UniMorphTag::Pass(ONE << 29);
const UniMorphTag UniMorphTag::Pos(ONE << 30);
const UniMorphTag UniMorphTag::Notpast(ONE << 31);
const UniMorphTag UniMorphTag::Conv(ONE << 32);
const UniMorphTag UniMorphTag::Mid(ONE << 33);
const UniMorphTag UniMorphTag::Digit(ONE << 34);

static ITag GENDER_MASK = UniMorphTag::Neut | UniMorphTag::Fem | UniMorphTag::Masc;
static ITag NUMBER_MASK = UniMorphTag::Sing | UniMorphTag::Plur;
static ITag CASE_MASK
    = UniMorphTag::Ins | UniMorphTag::Acc | UniMorphTag::Nom | UniMorphTag::Dat | UniMorphTag::Gen | UniMorphTag::Loc | UniMorphTag::Voc;
static ITag TENSE_MASK = UniMorphTag::Fut | UniMorphTag::Pres | UniMorphTag::Past | UniMorphTag::Notpast;
static ITag ANIM_MASK = UniMorphTag::Anim | UniMorphTag::Inan;

const std::vector<UniMorphTag> UniMorphTag::inner_runner = {
    UNKN, Masc, Fem,  Neut,  Anim, Inan, Sing, Plur,
    Ins,  Acc,  Nom,  Dat,   Gen,  Loc,  Voc,  Cmp,
    Sup,  Pos,  Fin,  Inf,   Conv, Imp,  Ind,  _1,
    _2,   _3,   Fut,  Past, Pres, Notpast, Short, Act,
    Pass, Mid,  Digit,
};

UniMorphTag::UniMorphTag(uint64_t val) : ITag(val, &UNI_MORPH_MAP)
{
}

UniMorphTag::UniMorphTag(const std::string & val) : ITag(val, &UNI_MORPH_MAP)
{
}

UniMorphTag::UniMorphTag() : ITag((uint64_t)0, &UNI_MORPH_MAP)
{
}

UniMorphTag UniMorphTag::getGender() const
{
    return this->intersect(GENDER_MASK);
}

UniMorphTag UniMorphTag::getNumber() const
{
    return this->intersect(NUMBER_MASK);
}

UniMorphTag UniMorphTag::getCase() const
{
    return this->intersect(CASE_MASK);
}

UniMorphTag UniMorphTag::getTense() const
{
    return this->intersect(TENSE_MASK);
}

UniMorphTag UniMorphTag::getAnimacy() const
{
    return this->intersect(ANIM_MASK);
}

void UniMorphTag::setGender(const UniMorphTag & gender)
{
    *this = intersect(~GENDER_MASK);
    *this |= gender;
}
void UniMorphTag::setNumber(const UniMorphTag & number)
{
    *this = intersect(~NUMBER_MASK);
    *this |= number;
}
void UniMorphTag::setCase(const UniMorphTag & cas)
{
    *this = intersect(~CASE_MASK);
    *this |= cas;
}
void UniMorphTag::setTense(const UniMorphTag & tense)
{
    *this = intersect(~TENSE_MASK);
    *this |= tense;
}

void UniMorphTag::setAnimacy(const UniMorphTag & anim)
{
    *this = intersect(~ANIM_MASK);
    *this |= anim;
}

UniMorphTag UniMorphTag::operator|(const UniMorphTag & o) const
{
    return UniMorphTag((uint64_t)this->ITag::operator|(o));
}

}