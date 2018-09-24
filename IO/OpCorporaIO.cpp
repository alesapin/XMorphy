#include "OpCorporaIO.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace io {
    const std::string OpCorporaIO::SAME_FEATURE("<~>");
    std::string OpCorporaIO::write(analyze::WordFormPtr wform) const {
        std::ostringstream oss;
        if (wform->getType() & base::TokenTypeTag::SEPR)
            return "";
        for (const auto& mi : wform->getMorphInfo()) {
            oss << wform->getWordForm().getRawString() << "\t";
            oss << mi.normalForm.toLowerCase().getRawString() << "\t";
            oss << mi.sp << "\t";
            oss << mi.tag;
            const std::vector<base::PhemTag>& pheminfo = wform->getPhemInfo();
            if (!pheminfo.empty()) {
                oss << "\t";
                base::PhemTag prev = base::PhemTag::UNKN;
                for (std::size_t i = 0; i < pheminfo.size(); ++i) {
                    if (pheminfo[i] != prev) {
                        if (i != 0)
                            oss << "|";
                        if (pheminfo[i] == base::PhemTag::PREFIX) {
                            oss << "*";
                        } else if (pheminfo[i] == base::PhemTag::SUFFIX) {
                            oss << "-";
                        } else if (pheminfo[i] == base::PhemTag::ENDING) {
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

    boost::property_tree::ptree OpCorporaIO::writeToJSON(analyze::WordFormPtr wform) const {
        namespace pt = boost::property_tree;
        pt::ptree array;
        if (wform->getType() & base::TokenTypeTag::SEPR)
            return array;

        std::ostringstream oss;
        for (auto& mi : wform->getMorphInfo()) {
            pt::ptree elem;
            elem.put("lemma", mi.normalForm.toLowerCase().getRawString());
            elem.put("speech_part", mi.sp);
            elem.put("case", mi.tag.getCase());
            elem.put("gender", mi.tag.getGender());
            elem.put("number", mi.tag.getNumber());
            elem.put("tense", mi.tag.getTense());
            elem.put("raw_tags", mi.tag);
            array.push_back(std::make_pair("", elem));
        }
        return array;
    }

} // namespace io
