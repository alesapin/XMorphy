#include "OpCorporaIO.h"
namespace io {

const std::string OpCorporaIO::SAME_FEATURE("<~>");
std::string OpCorporaIO::write(analyze::WordFormPtr wform) const {
	std::ostringstream oss;
    if (wform->getType() & base::TokenTypeTag::SEPR) return "";
    for (const auto& mi : wform->getMorphInfo()) {
        oss << wform->getWordForm().getRawString() << "\t";
        oss << mi.normalForm.toLowerCase().getRawString() << "\t";
        oss << mi.sp << "\t";
        oss << mi.tag;
        const std::vector<base::PhemTag> &pheminfo = wform->getPhemInfo();
        if (!pheminfo.empty()) {
            oss << "\t";
            base::PhemTag prev = base::PhemTag::UNKN;
            for (std::size_t i = 0; i < pheminfo.size(); ++i) {
                if (pheminfo[i] != prev) {
                    if (i != 0 ) oss << "|";
                    if (pheminfo[i] == base::PhemTag::PREFIX) {
                        oss << "*";
                    } else if ( pheminfo[i] == base::PhemTag::SUFFIX) {
                        oss << "-";
                    } else if ( pheminfo[i] == base::PhemTag::ENDING) {
                        oss << "+";
                    }
                    prev = pheminfo[i];
                }
                oss << wform->getWordForm()[i].getInnerRepr();
            }
        }
        oss << "\n";
    }
    std::string result = oss.str();
    result.pop_back();
    return result;
}

}
