#include "WordForm.h"
namespace analyze {
bool MorphInfo::operator<(const MorphInfo& other) const {
    return std::tie(normalForm, tag, sp) < std::tie(other.normalForm, other.tag, other.sp);
}

bool MorphInfo::operator==(const MorphInfo& other) const {
    return std::tie(normalForm, tag, sp) == std::tie(other.normalForm, other.tag, other.sp);
}

utils::UniString WordForm::toString() const {
    std::string result; //TODO FIX THIS SHIT
    result += "{";
    if (getType() == base::TokenTypeTag::WORD) {
        result += "\"" + inner.getRawString() + "\":[";
        for (const MorphInfo& info : morphInfos) {
            result += "{\"nf\":\"" + info.normalForm.getRawString() + "\",";
            result += "\"sp\":\"" + to_string(info.sp) + "\",";
            result += "\"mtag\":\"" + to_string(info.tag) + "\",";
            result += "\"prob\":" + std::to_string(info.probability) + ",";
            result += "\"amethod\":\"" + to_string(info.at) + "\"},";
        }
        result.pop_back();
    } else {
        result += "\"nw\":[";
        result += "{\"tt\":\"" + to_string(getType()) + "\",";
        result += "\"gtag\":\"" + to_string(getTag()) + "\"}";
    }
    result += "]}";
    return utils::UniString(result);
}
}
