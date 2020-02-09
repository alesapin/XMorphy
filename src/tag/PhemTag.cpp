#include "PhemTag.h"

namespace base {
static const boost::bimap<uint64_t, std::string> PHEM_MAP =
    boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(0x00, "UNKN")(0x01, "PREFIX")(0x02, "ROOT")(0x04, "SUFFIX")(0x08, "ENDING");

const PhemTag PhemTag::UNKN(uint64_t(0x00));
const PhemTag PhemTag::PREFIX(uint64_t(0x01));
const PhemTag PhemTag::ROOT(uint64_t(0x02));
const PhemTag PhemTag::SUFFIX(uint64_t(0x04));
const PhemTag PhemTag::ENDING(uint64_t(0x08));

const std::vector<PhemTag> PhemTag::inner_runner = {
    UNKN,
    PREFIX,
    ROOT,
    SUFFIX,
    ENDING,
};

PhemTag::PhemTag(uint64_t val)
    : ITag(val, &PHEM_MAP) {
}

PhemTag::PhemTag(const std::string& val)
    : ITag(val, &PHEM_MAP) {
}

PhemTag::PhemTag()
    : ITag((uint64_t)0, &PHEM_MAP) {
}
}
