#include "SpeechPartTag.h"
namespace X
{
static const boost::bimap<uint64_t, std::string> SPEECH_PART_MAP
    = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(0x00, "UNKN")(0x01, "NOUN")(0x02, "ADJF")(0x04, "ADJS")(
        0x08, "COMP")(0x10, "VERB")(0x20, "INFN")(0x40, "PRTF")(0x80, "PRTS")(0x100, "GRND")(0x200, "NUMR")(0x400, "ADVB")(0x800, "NPRO")(
        0x1000, "PRED")(0x2000, "PREP")(0x4000, "CONJ")(0x8000, "PRCL")(0x10000, "INTJ");

const SpeechPartTag SpeechPartTag::UNKN(uint64_t(0x00));
const SpeechPartTag SpeechPartTag::NOUN(uint64_t(0x01));
const SpeechPartTag SpeechPartTag::ADJF(uint64_t(0x02));
const SpeechPartTag SpeechPartTag::ADJS(uint64_t(0x04));
const SpeechPartTag SpeechPartTag::COMP(uint64_t(0x08));
const SpeechPartTag SpeechPartTag::VERB(uint64_t(0x10));
const SpeechPartTag SpeechPartTag::INFN(uint64_t(0x20));
const SpeechPartTag SpeechPartTag::PRTF(uint64_t(0x40));
const SpeechPartTag SpeechPartTag::PRTS(uint64_t(0x80));
const SpeechPartTag SpeechPartTag::GRND(uint64_t(0x100));
const SpeechPartTag SpeechPartTag::NUMR(uint64_t(0x200));
const SpeechPartTag SpeechPartTag::ADVB(uint64_t(0x400));
const SpeechPartTag SpeechPartTag::NPRO(uint64_t(0x800));
const SpeechPartTag SpeechPartTag::PRED(uint64_t(0x1000));
const SpeechPartTag SpeechPartTag::PREP(uint64_t(0x2000));
const SpeechPartTag SpeechPartTag::CONJ(uint64_t(0x4000));
const SpeechPartTag SpeechPartTag::PRCL(uint64_t(0x8000));
const SpeechPartTag SpeechPartTag::INTJ(uint64_t(0x10000));


const std::vector<SpeechPartTag> SpeechPartTag::inner_runner = {
    UNKN,
    NOUN,
    ADJF,
    ADJS,
    COMP,
    VERB,
    INFN,
    PRTF,
    PRTS,
    GRND,
    NUMR,
    ADVB,
    NPRO,
    PRED,
    PREP,
    CONJ,
    PRCL,
    INTJ,
};

SpeechPartTag::SpeechPartTag(uint64_t val) : ITag(val, &SPEECH_PART_MAP)
{
}

SpeechPartTag::SpeechPartTag(const std::string & val) : ITag(val, &SPEECH_PART_MAP)
{
}

SpeechPartTag::SpeechPartTag() : ITag((uint64_t)0, &SPEECH_PART_MAP)
{
}
}
