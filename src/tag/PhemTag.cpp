#include "PhemTag.h"

namespace X
{
static const boost::bimap<uint64_t, std::string> PHEM_MAP
    = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(0x00, "UNKN")(0x01, "PREF")(0x02, "ROOT")(0x04, "SUFF")(
        0x08, "END")(0x10, "LINK")(0x20, "HYPH")(0x40, "POSTFIX")(0x80, "B-SUFF")(0x100, "B-PREF")(0x200, "B-ROOT");

const PhemTag PhemTag::UNKN(uint64_t(0x00));
const PhemTag PhemTag::PREF(uint64_t(0x01));
const PhemTag PhemTag::ROOT(uint64_t(0x02));
const PhemTag PhemTag::SUFF(uint64_t(0x04));
const PhemTag PhemTag::END(uint64_t(0x08));
const PhemTag PhemTag::LINK(uint64_t(0x10));
const PhemTag PhemTag::HYPH(uint64_t(0x20));
const PhemTag PhemTag::POSTFIX(uint64_t(0x40));
const PhemTag PhemTag::B_SUFF(uint64_t(0x80));
const PhemTag PhemTag::B_PREF(uint64_t(0x100));
const PhemTag PhemTag::B_ROOT(uint64_t(0x200));

const std::vector<PhemTag> PhemTag::inner_runner = {
    UNKN,
    PREF,
    ROOT,
    SUFF,
    END,
    LINK,
    HYPH,
    POSTFIX,
    B_SUFF,
    B_PREF,
    B_ROOT,
};

PhemTag::PhemTag(uint64_t val) : ITag(val, &PHEM_MAP)
{
}

PhemTag::PhemTag(const std::string & val) : ITag(val, &PHEM_MAP)
{
}

PhemTag::PhemTag() : ITag((uint64_t)0, &PHEM_MAP)
{
}
}
