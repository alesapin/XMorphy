#pragma once
#include "ITag.h"
namespace X
{
struct GraphemTag : public ITag
{
private:
    GraphemTag(uint64_t val);
    GraphemTag(const std::string & val);
    static const std::vector<GraphemTag> inner_runner;

public:
    static const GraphemTag UNKN;
    //word tags
    static const GraphemTag CYRILLIC;
    static const GraphemTag LATIN;
    static const GraphemTag UPPER_CASE;
    static const GraphemTag LOWER_CASE;
    static const GraphemTag MIXED;
    static const GraphemTag CAP_START;
    static const GraphemTag ABBR;
    static const GraphemTag NAM_ENT;
    static const GraphemTag MULTI_WORD;
    static const GraphemTag SINGLE_WORD;
    static const GraphemTag MULTI_ENC;
    //punct tags
    static const GraphemTag COMMA;
    static const GraphemTag DOT;
    static const GraphemTag COLON;
    static const GraphemTag SEMICOLON;
    static const GraphemTag QUESTION_MARK;
    static const GraphemTag EXCLAMATION_MARK;
    static const GraphemTag THREE_DOTS;
    static const GraphemTag QUOTE;
    static const GraphemTag DASH;
    static const GraphemTag PARENTHESIS_L;
    static const GraphemTag PARENTHESIS_R;
    static const GraphemTag UNCOMMON_PUNCT;
    static const GraphemTag PUNCT_GROUP;
    static const GraphemTag LOWER_DASH;
    //number tags
    static const GraphemTag DECIMAL;
    static const GraphemTag BINARY;
    static const GraphemTag OCT;
    static const GraphemTag HEX;
    //separator tags
    static const GraphemTag SPACE;
    static const GraphemTag TAB;
    static const GraphemTag NEW_LINE;
    static const GraphemTag CR;
    static const GraphemTag SINGLE_SEP;
    static const GraphemTag MULTI_SEP;

    static const GraphemTag CONNECTED;
    static std::vector<GraphemTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<GraphemTag>::const_iterator end() { return inner_runner.end(); }

    GraphemTag operator|(const GraphemTag & o) const { return GraphemTag((uint64_t)this->ITag::operator|(o)); }
    bool operator==(const GraphemTag & other) const { return value == other.value && name_map == other.name_map; }
    bool operator!=(const GraphemTag & other) const { return !this->operator==(other); }
    bool operator<(const GraphemTag & other) const { return value < other.value; }
    bool operator>(const GraphemTag & other) const { return value > other.value; }
};
}
