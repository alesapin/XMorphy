#include "UniSPTag.h"
namespace X
{
static const boost::bimap<uint64_t, std::string> UNI_SP_MAP
    = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(0x00, "X")(0x01, "ADJ")(0x02, "ADV")(0x04, "INTJ")(
        0x08, "NOUN")(0x10, "PROPN")(0x20, "VERB")(0x40, "ADP")(0x80, "AUX")(0x100, "CONJ")(0x200, "SCONJ")(0x400, "DET")(0x800, "NUM")(
        0x1000, "PART")(0x2000, "PRON")(0x4000, "PUNCT")(0x8000, "H")(0x10000, "R")(0x20000, "Q")(0x40000, "SYM");

const UniSPTag UniSPTag::X(uint64_t(0x00));
const UniSPTag UniSPTag::ADJ(uint64_t(0x01));
const UniSPTag UniSPTag::ADV(uint64_t(0x02));
const UniSPTag UniSPTag::INTJ(uint64_t(0x04));
const UniSPTag UniSPTag::NOUN(uint64_t(0x08));
const UniSPTag UniSPTag::PROPN(uint64_t(0x10));
const UniSPTag UniSPTag::VERB(uint64_t(0x20));
const UniSPTag UniSPTag::ADP(uint64_t(0x40));
const UniSPTag UniSPTag::AUX(uint64_t(0x80));
const UniSPTag UniSPTag::CONJ(uint64_t(0x100));
const UniSPTag UniSPTag::SCONJ(uint64_t(0x200));
const UniSPTag UniSPTag::DET(uint64_t(0x400));
const UniSPTag UniSPTag::NUM(uint64_t(0x800));
const UniSPTag UniSPTag::PART(uint64_t(0x1000));
const UniSPTag UniSPTag::PRON(uint64_t(0x2000));
const UniSPTag UniSPTag::PUNCT(uint64_t(0x4000));
const UniSPTag UniSPTag::H(uint64_t(0x8000));
const UniSPTag UniSPTag::R(uint64_t(0x10000));
const UniSPTag UniSPTag::Q(uint64_t(0x20000));
const UniSPTag UniSPTag::SYM(uint64_t(0x40000));

const std::vector<UniSPTag> UniSPTag::inner_runner = {
    X, ADJ, ADV, INTJ, NOUN, PROPN, VERB, ADP, AUX, CONJ, SCONJ, DET, NUM, PART, PRON, PUNCT, H, R, Q, SYM,
};

UniSPTag::UniSPTag(uint64_t val) : ITag(val, &UNI_SP_MAP)
{
}

UniSPTag::UniSPTag(const std::string & val) : ITag(val, &UNI_SP_MAP)
{
}

UniSPTag::UniSPTag() : ITag((uint64_t)0, &UNI_SP_MAP)
{
}

}
