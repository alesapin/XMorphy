#ifndef PHEM_TAG_H
#define PHEM_TAG_H
#include "ITag.h"
namespace base {

struct PhemTag : public ITag {
private:
    PhemTag(uint64_t val);
    static const std::vector<PhemTag> inner_runner;

public:
    static const PhemTag UNKN;
    static const PhemTag PREFIX;
    static const PhemTag ROOT;
    static const PhemTag SUFFIX;
    static const PhemTag ENDING;

    static std::vector<PhemTag>::const_iterator begin() {
        return inner_runner.begin();
    }

    static std::vector<PhemTag>::const_iterator end() {
        return inner_runner.end();
    }

    static constexpr std::size_t size() {
        return 5;
    }

    static PhemTag get(std::size_t index) {
        return inner_runner[index];
    }
    static std::size_t get(PhemTag t) {
        return std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin();
    }

    PhemTag(const std::string& val);
    PhemTag();
};
}
#endif
