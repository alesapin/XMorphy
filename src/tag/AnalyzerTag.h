#ifndef _ANALYZER_TAG_H
#define _ANALYZER_TAG_H
#include "ITag.h"
namespace base {
struct AnalyzerTag : public ITag {
private:
    AnalyzerTag(uint128_t val);
    static const std::vector<AnalyzerTag> inner_runner;

public:
    static const AnalyzerTag UNKN;
    static const AnalyzerTag DICT;
    static const AnalyzerTag PREF;
    static const AnalyzerTag SUFF;
    static const AnalyzerTag HYPH;
    static std::vector<AnalyzerTag>::const_iterator begin() {
        return inner_runner.begin();
    }
    static std::vector<AnalyzerTag>::const_iterator end() {
        return inner_runner.end();
    }
    AnalyzerTag(const std::string& val);
    AnalyzerTag();
    bool operator==(const AnalyzerTag& other) const {
        return value == other.value && name_map == other.name_map;
    }
    bool operator!=(const AnalyzerTag& other) const {
        return !this->operator==(other);
    }
    bool operator<(const AnalyzerTag& other) const {
        return value < other.value;
    }
    bool operator>(const AnalyzerTag& other) const {
        return value > other.value;
    }
};
}
#endif
