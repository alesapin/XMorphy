#include <xmorphy/tag/AnalyzerTag.h>

namespace X
{
static const uint64_t ONE = 1;

static const boost::bimap<uint64_t, std::string> ANALYZER_TAG_MAP = boost::assign::list_of<boost::bimap<uint64_t, std::string>::relation>(
    0, "UNKN")(ONE << 0, "DICT")(ONE << 1, "PREF")(ONE << 2, "SUFF")(ONE << 3, "HYPH");

const AnalyzerTag AnalyzerTag::UNKN(uint64_t(0));
const AnalyzerTag AnalyzerTag::DICT(ONE << 0);
const AnalyzerTag AnalyzerTag::PREF(ONE << 1);
const AnalyzerTag AnalyzerTag::SUFF(ONE << 2);
const AnalyzerTag AnalyzerTag::HYPH(ONE << 3);
const std::vector<AnalyzerTag> AnalyzerTag::inner_runner = {
    UNKN,
    DICT,
    PREF,
    SUFF,
    HYPH,
};
AnalyzerTag::AnalyzerTag(uint64_t val) : ITag(val, &ANALYZER_TAG_MAP)
{
}
AnalyzerTag::AnalyzerTag(const std::string & val) : ITag(val, &ANALYZER_TAG_MAP)
{
}
AnalyzerTag::AnalyzerTag() : ITag((uint64_t)0, &ANALYZER_TAG_MAP)
{
}
}
