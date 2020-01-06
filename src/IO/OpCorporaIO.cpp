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
            oss << mi.tag << "\t";
            oss << mi.at;
            const std::vector<base::PhemTag>& pheminfo = wform->getPhemInfo();
            if (!pheminfo.empty()) {
                oss << "\t";
                oss << writePhemInfo(wform);
            }
            oss << "\n";
        }
        std::string result = oss.str();
        result.pop_back();
        return result;
    }

    std::string OpCorporaIO::writePhemInfo(analyze::WordFormPtr wform) const {
        std::ostringstream oss;
        const std::vector<base::PhemTag>& pheminfo = wform->getPhemInfo();
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
        return oss.str();
    }

    std::vector<base::PhemTag> OpCorporaIO::readPhemInfo(const std::string& phemInfo) const {
        std::vector<base::PhemTag> result;
        if(phemInfo.empty()) return result;
        std::vector<std::string> parts;
        boost::split(parts, phemInfo, boost::is_any_of("|"));
        for (const auto & part : parts) {
            base::PhemTag current;
            if (part[0] == '*')
                current = base::PhemTag::PREFIX;
            else if (part[0] == '-')
                current = base::PhemTag::SUFFIX;
            else if (part[0] == '+')
                current = base::PhemTag::ENDING;
            else
                current = base::PhemTag::ROOT;

            utils::UniString unipart(part);
            size_t len = unipart.length() - 1;
            if (current == base::PhemTag::ROOT)
                len++;
            for(size_t i = 0; i < len; ++i)
                result.push_back(current);
        }

        return result;
    }

    boost::property_tree::ptree OpCorporaIO::writeToJSON(analyze::WordFormPtr wform) const {
        namespace pt = boost::property_tree;
        pt::ptree array;
        if (wform->getType() & base::TokenTypeTag::SEPR)
            return array;

        std::ostringstream oss;
        size_t index = 0;
        for (auto& mi : wform->getMorphInfo()) {
            pt::ptree elem;
            elem.put("lemma", mi.normalForm.toLowerCase().getRawString());
            elem.put("speech_part", mi.sp);
            elem.put("case", mi.tag.getCase());
            elem.put("gender", mi.tag.getGender());
            elem.put("number", mi.tag.getNumber());
            elem.put("tense", mi.tag.getTense());
            elem.put("raw_tags", mi.tag);
            elem.put("phem_info", writePhemInfo(wform));
            array.push_back(std::make_pair(std::to_string(index++), elem));
        }
        return array;
    }

} // namespace io
