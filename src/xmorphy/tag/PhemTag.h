#pragma once
#include "ITag.h"
namespace X
{
struct PhemTag : public ITag
{
private:
    PhemTag(uint64_t val);
    static const std::vector<PhemTag> inner_runner;

public:
    static const PhemTag UNKN;
    static const PhemTag PREF;
    static const PhemTag ROOT;
    static const PhemTag SUFF;
    static const PhemTag END;
    static const PhemTag LINK;
    static const PhemTag HYPH;
    static const PhemTag POSTFIX;
    static const PhemTag B_SUFF;
    static const PhemTag B_PREF;
    static const PhemTag B_ROOT;

    static std::vector<PhemTag>::const_iterator begin() { return inner_runner.begin(); }

    static std::vector<PhemTag>::const_iterator end() { return inner_runner.end(); }

    static constexpr std::size_t size() { return 11; }

    static PhemTag get(std::size_t index) { return inner_runner[index]; }
    static std::size_t get(PhemTag t) { return std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin(); }

    bool operator==(const PhemTag & other) const { return value == other.value && name_map == other.name_map; }

    PhemTag(const std::string & val);
    PhemTag();
};
}
