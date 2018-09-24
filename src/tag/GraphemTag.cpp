#include "GraphemTag.h"
namespace base {
static const boost::bimap<uint128_t, std::string> NAME_MAP =
    boost::assign::list_of<boost::bimap<uint128_t, std::string>::relation>
    (0x00       ,  "UNKN"              )
    (0x01       ,  "CYRILLIC"          )
    (0x02       ,  "LATIN"             )
    (0x04       ,  "UPPER_CASE"        )
    (0x08       ,  "LOWER_CASE"        )
    (0x10       ,  "MIXED"             )
    (0x20       ,  "CAP_START"         )
    (0x40       ,  "ABBR"              )
    (0x80       ,  "NAM_ENT"           )
    (0x100      ,  "MULTI_WORD"        )
    (0x200      ,  "SINGLE_WORD"       )
    (0x400      ,  "MULTI_ENC"         )
    (0x800      ,  "COMMA"             )
    (0x1000     ,  "DOT"               )
    (0x2000     ,  "COLON"             )
    (0x4000     ,  "SEMICOLON"         )
    (0x8000     ,  "QUESTION_MARK"     )
    (0x10000    ,  "EXCLAMATION_MARK"  )
    (0x20000    ,  "THREE_DOTS"        )
    (0x40000    ,  "QUOTE"             )
    (0x80000    ,  "DASH"              )
    (0x100000   ,  "PARENTHESIS_L"     )
    (0x200000   ,  "PARENTHESIS_R"     )
    (0x400000   ,  "UNCOMMON_PUNCT"    )
    (0x800000   ,  "PUNCT_GROUP"       )
    (0x1000000  ,  "DECIMAL"           )
    (0x2000000  ,  "BINARY"            )
    (0x4000000  ,  "OCT"               )
    (0x8000000  ,  "HEX"               )
    (0x10000000 ,  "SPACE"             )
    (0x20000000 ,  "TAB"               )
    (0x40000000 ,  "NEW_LINE"          )
    (0x80000000 ,  "CR"                )
    (0x100000000,  "SINGLE_SEP"        )
    (0x200000000,  "MULTI_SEP"         )
    (0x400000000,  "LOWER_DASH"        );

const GraphemTag GraphemTag::UNKN            (uint128_t(0x00       ));
const GraphemTag GraphemTag::CYRILLIC        (uint128_t(0x01       ));
const GraphemTag GraphemTag::LATIN           (uint128_t(0x02       ));
const GraphemTag GraphemTag::UPPER_CASE      (uint128_t(0x04       ));
const GraphemTag GraphemTag::LOWER_CASE      (uint128_t(0x08       ));
const GraphemTag GraphemTag::MIXED           (uint128_t(0x10       ));
const GraphemTag GraphemTag::CAP_START       (uint128_t(0x20       ));
const GraphemTag GraphemTag::ABBR            (uint128_t(0x40       ));
const GraphemTag GraphemTag::NAM_ENT         (uint128_t(0x80       ));
const GraphemTag GraphemTag::MULTI_WORD      (uint128_t(0x100      ));
const GraphemTag GraphemTag::SINGLE_WORD     (uint128_t(0x200      ));
const GraphemTag GraphemTag::MULTI_ENC       (uint128_t(0x400      ));
const GraphemTag GraphemTag::COMMA           (uint128_t(0x800      ));
const GraphemTag GraphemTag::DOT             (uint128_t(0x1000     ));
const GraphemTag GraphemTag::COLON           (uint128_t(0x2000     ));
const GraphemTag GraphemTag::SEMICOLON       (uint128_t(0x4000     ));
const GraphemTag GraphemTag::QUESTION_MARK   (uint128_t(0x8000     ));
const GraphemTag GraphemTag::EXCLAMATION_MARK(uint128_t(0x10000    ));
const GraphemTag GraphemTag::THREE_DOTS      (uint128_t(0x20000    ));
const GraphemTag GraphemTag::QUOTE           (uint128_t(0x40000    ));
const GraphemTag GraphemTag::DASH            (uint128_t(0x80000    ));
const GraphemTag GraphemTag::PARENTHESIS_L   (uint128_t(0x100000   ));
const GraphemTag GraphemTag::PARENTHESIS_R   (uint128_t(0x200000   ));
const GraphemTag GraphemTag::UNCOMMON_PUNCT  (uint128_t(0x400000   ));
const GraphemTag GraphemTag::PUNCT_GROUP     (uint128_t(0x800000   ));
const GraphemTag GraphemTag::DECIMAL         (uint128_t(0x1000000  ));
const GraphemTag GraphemTag::BINARY          (uint128_t(0x2000000  ));
const GraphemTag GraphemTag::OCT             (uint128_t(0x4000000  ));
const GraphemTag GraphemTag::HEX             (uint128_t(0x8000000  ));
const GraphemTag GraphemTag::SPACE           (uint128_t(0x10000000 ));
const GraphemTag GraphemTag::TAB             (uint128_t(0x20000000 ));
const GraphemTag GraphemTag::NEW_LINE        (uint128_t(0x40000000 ));
const GraphemTag GraphemTag::CR              (uint128_t(0x80000000 ));
const GraphemTag GraphemTag::SINGLE_SEP      (uint128_t(0x100000000));
const GraphemTag GraphemTag::MULTI_SEP       (uint128_t(0x200000000));
const GraphemTag GraphemTag::LOWER_DASH      (uint128_t(0x400000000));

const std::vector<GraphemTag> GraphemTag::inner_runner = {
    UNKN            ,
    CYRILLIC        ,
    LATIN           ,
    UPPER_CASE      ,
    LOWER_CASE      ,
    MIXED           ,
    CAP_START       ,
    ABBR            ,
    NAM_ENT         ,
    MULTI_WORD      ,
    SINGLE_WORD     ,
    MULTI_ENC       ,
    COMMA           ,
    DOT             ,
    COLON           ,
    SEMICOLON       ,
    QUESTION_MARK   ,
    EXCLAMATION_MARK,
    THREE_DOTS      ,
    QUOTE           ,
    DASH            ,
    PARENTHESIS_L   ,
    PARENTHESIS_R   ,
    UNCOMMON_PUNCT  ,
    PUNCT_GROUP     ,
    DECIMAL         ,
    BINARY          ,
    OCT             ,
    HEX             ,
    SPACE           ,
    TAB             ,
    NEW_LINE        ,
    CR              ,
    SINGLE_SEP      ,
    MULTI_SEP       ,
    LOWER_DASH      ,
};

GraphemTag::GraphemTag(uint128_t val): ITag(val, &NAME_MAP) {}
GraphemTag::GraphemTag(const std::string &val): ITag(val, &NAME_MAP) {}
}

