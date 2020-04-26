#include "RusCorpMorphTag.h"
namespace X
{
static const uint64_t ONE = 1;
static const boost::bimap<uint64_t, std::string> RUS_CORP_MORPH_MAP = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(
    0x00, "UNKN")(ONE << 0, "m")(ONE << 1, "f")(ONE << 2, "m-f")(ONE << 3, "n")(ONE << 4, "anim")(ONE << 5, "inan")(ONE << 6, "sg")(
    ONE << 7, "pl")(ONE << 8, "nom")(ONE << 9, "gen")(ONE << 10, "dat")(ONE << 11, "acc")(ONE << 12, "ins")(ONE << 13, "loc")(
    ONE << 14, "gen2")(ONE << 15, "acc2")(ONE << 16, "loc2")(ONE << 17, "voc")(ONE << 18, "adnum")(ONE << 19, "comp")(ONE << 20, "comp2")(
    ONE << 21, "supr")(ONE << 22, "inf")(ONE << 23, "partcp")(ONE << 24, "ger")(ONE << 25, "indic")(ONE << 26, "imper")(
    ONE << 27, "imper2")(ONE << 28, "1p")(ONE << 29, "2p")(ONE << 30, "3p")(ONE << 31, "praet")(ONE << 32, "praes")(ONE << 33, "fut")(
    ONE << 34, "brev")(ONE << 35, "plen")(ONE << 36, "act")(ONE << 37, "pass")(ONE << 38, "med")(ONE << 39, "intr")(ONE << 40, "tran")(
    ONE << 41, "persn")(ONE << 42, "patrn")(ONE << 43, "famn")(ONE << 44, "zoon")(ONE << 45, "0")(ONE << 46, "abbr")(ONE << 47, "distort")(
    ONE << 48, "ciph")(ONE << 49, "anom")(ONE << 50, "pf")(ONE << 51, "ipf");

const RusCorpMorphTag RusCorpMorphTag::UNKN((uint64_t)0);
const RusCorpMorphTag RusCorpMorphTag::m(ONE << 0);
const RusCorpMorphTag RusCorpMorphTag::f(ONE << 1);
const RusCorpMorphTag RusCorpMorphTag::m_f(ONE << 2);
const RusCorpMorphTag RusCorpMorphTag::n(ONE << 3);
const RusCorpMorphTag RusCorpMorphTag::anim(ONE << 4);
const RusCorpMorphTag RusCorpMorphTag::inan(ONE << 5);
const RusCorpMorphTag RusCorpMorphTag::sg(ONE << 6);
const RusCorpMorphTag RusCorpMorphTag::pl(ONE << 7);
const RusCorpMorphTag RusCorpMorphTag::nom(ONE << 8);
const RusCorpMorphTag RusCorpMorphTag::gen(ONE << 9);
const RusCorpMorphTag RusCorpMorphTag::dat(ONE << 10);
const RusCorpMorphTag RusCorpMorphTag::acc(ONE << 11);
const RusCorpMorphTag RusCorpMorphTag::ins(ONE << 12);
const RusCorpMorphTag RusCorpMorphTag::loc(ONE << 13);
const RusCorpMorphTag RusCorpMorphTag::gen2(ONE << 14);
const RusCorpMorphTag RusCorpMorphTag::acc2(ONE << 15);
const RusCorpMorphTag RusCorpMorphTag::loc2(ONE << 16);
const RusCorpMorphTag RusCorpMorphTag::voc(ONE << 17);
const RusCorpMorphTag RusCorpMorphTag::adnum(ONE << 18);
const RusCorpMorphTag RusCorpMorphTag::comp(ONE << 19);
const RusCorpMorphTag RusCorpMorphTag::comp2(ONE << 20);
const RusCorpMorphTag RusCorpMorphTag::supr(ONE << 21);
const RusCorpMorphTag RusCorpMorphTag::inf(ONE << 22);
const RusCorpMorphTag RusCorpMorphTag::partcp(ONE << 23);
const RusCorpMorphTag RusCorpMorphTag::ger(ONE << 24);
const RusCorpMorphTag RusCorpMorphTag::indic(ONE << 25);
const RusCorpMorphTag RusCorpMorphTag::imper(ONE << 26);
const RusCorpMorphTag RusCorpMorphTag::imper2(ONE << 27);
const RusCorpMorphTag RusCorpMorphTag::_1p(ONE << 28);
const RusCorpMorphTag RusCorpMorphTag::_2p(ONE << 29);
const RusCorpMorphTag RusCorpMorphTag::_3p(ONE << 30);
const RusCorpMorphTag RusCorpMorphTag::praet(ONE << 31);
const RusCorpMorphTag RusCorpMorphTag::praes(ONE << 32);
const RusCorpMorphTag RusCorpMorphTag::fut(ONE << 33);
const RusCorpMorphTag RusCorpMorphTag::brev(ONE << 34);
const RusCorpMorphTag RusCorpMorphTag::plen(ONE << 35);
const RusCorpMorphTag RusCorpMorphTag::act(ONE << 36);
const RusCorpMorphTag RusCorpMorphTag::pass(ONE << 37);
const RusCorpMorphTag RusCorpMorphTag::med(ONE << 38);
const RusCorpMorphTag RusCorpMorphTag::intr(ONE << 39);
const RusCorpMorphTag RusCorpMorphTag::tran(ONE << 40);
const RusCorpMorphTag RusCorpMorphTag::persn(ONE << 41);
const RusCorpMorphTag RusCorpMorphTag::patrn(ONE << 42);
const RusCorpMorphTag RusCorpMorphTag::famn(ONE << 43);
const RusCorpMorphTag RusCorpMorphTag::zoon(ONE << 44);
const RusCorpMorphTag RusCorpMorphTag::_0(ONE << 45);
const RusCorpMorphTag RusCorpMorphTag::abbr(ONE << 46);
const RusCorpMorphTag RusCorpMorphTag::distort(ONE << 47);
const RusCorpMorphTag RusCorpMorphTag::ciph(ONE << 48);
const RusCorpMorphTag RusCorpMorphTag::anom(ONE << 49);
const RusCorpMorphTag RusCorpMorphTag::pf(ONE << 50);
const RusCorpMorphTag RusCorpMorphTag::ipf(ONE << 51);

const std::vector<RusCorpMorphTag> RusCorpMorphTag::inner_runner = {
    UNKN, m,     f,    m_f,   n,    anim, inan,   sg,    pl,    nom,   gen,    dat,  acc,     ins,  loc,   gen2,  acc2, loc2,
    voc,  adnum, comp, comp2, supr, inf,  partcp, ger,   indic, imper, imper2, _1p,  _2p,     _3p,  praet, praes, fut,  brev,
    plen, act,   pass, med,   intr, tran, persn,  patrn, famn,  zoon,  _0,     abbr, distort, ciph, anom,  pf,    ipf,
};
RusCorpMorphTag::RusCorpMorphTag(uint64_t val) : ITag(val, &RUS_CORP_MORPH_MAP)
{
}

RusCorpMorphTag::RusCorpMorphTag(const std::string & val) : ITag(val, &RUS_CORP_MORPH_MAP)
{
}

RusCorpMorphTag::RusCorpMorphTag() : ITag((uint64_t)0, &RUS_CORP_MORPH_MAP)
{
}
}
