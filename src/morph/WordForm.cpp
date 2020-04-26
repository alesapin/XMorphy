#include "WordForm.h"
namespace X {
bool MorphInfo::operator<(const MorphInfo& other) const {
    return std::tie(normalForm, tag, sp) < std::tie(other.normalForm, other.tag, other.sp);
}

bool MorphInfo::operator==(const MorphInfo& other) const {
    return std::tie(normalForm, tag, sp) == std::tie(other.normalForm, other.tag, other.sp);
}

}
