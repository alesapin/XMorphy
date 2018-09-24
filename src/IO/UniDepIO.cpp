#include "UniDepIO.h"
namespace io {
std::string UniDepIO::write(analyze::WordFormPtr wform) const {
    std::ostringstream oss;
    if (wform->getType() & base::TokenTypeTag::SEPR) return "";
    for (const auto& mi : wform->getMorphInfo()) {
        oss << wform->getWordForm().getRawString() << "\t";
        oss << mi.normalForm.toLowerCase().getRawString() << "\t";
        oss << mi.usp << "\t";
        oss << mi.utag;
        if (fool) {
            oss << "\t" << mi.at << "\t";
            oss << mi.probability << "\t";
        }
        oss << "\n";
    }
    std::string result = oss.str();
    result.pop_back();
    return result;
}

}
