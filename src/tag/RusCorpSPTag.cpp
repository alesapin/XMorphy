#include "RusCorpSPTag.h"
namespace base {
static const boost::bimap<uint128_t, std::string> RUS_CORP_MAP =
    boost::assign::list_of<boost::bimap<uint128_t, std::string>::relation>
(0x00     ,     "UNKN"      )
(0x01     ,     "S"         )
(0x02     ,     "A"         )
(0x04     ,     "NUM"       )
(0x08     ,     "ANUM"      )
(0x10     ,     "V"         )
(0x20     ,     "ADV"       )
(0x40     ,     "PRAEDIC"   )
(0x80     ,     "PARENTH"   )
(0x100    ,     "S-PRO"      )
(0x200    ,     "A-PRO"      )
(0x400    ,     "ADV-PRO"    )
(0x800    ,     "PRAEDIC-PRO")
(0x1000   ,     "PR"        )
(0x2000   ,     "CONJ"      )
(0x4000   ,     "PART"      )
(0x8000   ,     "INTJ"      )
(0x10000   ,     "INIT"      );

const RusCorpSPTag RusCorpSPTag::UNKN      (uint128_t(0x00   ));
const RusCorpSPTag RusCorpSPTag::S         (uint128_t(0x01   ));
const RusCorpSPTag RusCorpSPTag::A         (uint128_t(0x02   ));
const RusCorpSPTag RusCorpSPTag::NUM       (uint128_t(0x04   ));
const RusCorpSPTag RusCorpSPTag::ANUM      (uint128_t(0x08   ));
const RusCorpSPTag RusCorpSPTag::V         (uint128_t(0x10   ));
const RusCorpSPTag RusCorpSPTag::ADV       (uint128_t(0x20   ));
const RusCorpSPTag RusCorpSPTag::PRAEDIC   (uint128_t(0x40   ));
const RusCorpSPTag RusCorpSPTag::PARENTH   (uint128_t(0x80   ));
const RusCorpSPTag RusCorpSPTag::SPRO      (uint128_t(0x100  ));
const RusCorpSPTag RusCorpSPTag::APRO      (uint128_t(0x200  ));
const RusCorpSPTag RusCorpSPTag::ADVPRO    (uint128_t(0x400  ));
const RusCorpSPTag RusCorpSPTag::PRAEDICPRO(uint128_t(0x800  ));
const RusCorpSPTag RusCorpSPTag::PR        (uint128_t(0x1000 ));
const RusCorpSPTag RusCorpSPTag::CONJ      (uint128_t(0x2000 ));
const RusCorpSPTag RusCorpSPTag::PART      (uint128_t(0x4000 ));
const RusCorpSPTag RusCorpSPTag::INTJ      (uint128_t(0x8000 ));
const RusCorpSPTag RusCorpSPTag::INIT      (uint128_t(0x8000 ));


const std::vector<RusCorpSPTag> RusCorpSPTag::inner_runner = {
    UNKN,
    S,
    A,
    NUM,
    ANUM,
    V,
    ADV,
    PRAEDIC,
    PARENTH,
    SPRO,
    APRO,
    ADVPRO,
    PRAEDICPRO,
    PR,
    CONJ,
    PART,
    INTJ,
	INIT,
};

RusCorpSPTag::RusCorpSPTag(uint128_t val): ITag(val, &RUS_CORP_MAP) {}

RusCorpSPTag::RusCorpSPTag(const std::string &val): ITag(val, &RUS_CORP_MAP) {}

RusCorpSPTag::RusCorpSPTag(): ITag((uint128_t)0, &RUS_CORP_MAP) {}

}
