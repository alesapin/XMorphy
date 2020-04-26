#include "OpCorporaIO.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
namespace io {

std::string OpCorporaIO::write(analyze::WordFormPtr wform) const {
    std::ostringstream oss;
    if (wform->getType() & base::TokenTypeTag::SEPR)
        return "";
    for (const auto& mi : wform->getMorphInfo()) {
        oss << wform->getWordForm().getRawString() << "\t";
        oss << mi.normalForm.toLowerCase().getRawString() << "\t";
        oss << mi.sp << "\t";
        oss << mi.tag << "\t";
        oss << mi.at << "\t";
        oss << mi.probability;
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

static std::string printPhemTag(base::PhemTag tag)
{
    if (tag == base::PhemTag::B_SUFF)
        return to_string(base::PhemTag::SUFF);
    if (tag == base::PhemTag::B_ROOT)
        return to_string(base::PhemTag::ROOT);
    if (tag == base::PhemTag::B_PREF)
        return to_string(base::PhemTag::PREF);
    return to_string(tag);
}

static bool tagsEqual(base::PhemTag left, base::PhemTag right) {
    if (left == base::PhemTag::B_PREF && right == base::PhemTag::B_PREF)
        return false;
    if (left == base::PhemTag::B_ROOT && right == base::PhemTag::B_ROOT)
        return false;
    if (left == base::PhemTag::B_SUFF && right == base::PhemTag::B_SUFF)
        return false;

    if (left == base::PhemTag::B_PREF && right == base::PhemTag::PREF)
        return true;
    if (left == base::PhemTag::B_ROOT && right == base::PhemTag::ROOT)
        return true;
    if (left == base::PhemTag::B_SUFF && right == base::PhemTag::SUFF)
        return true;

    return left == right;
}

std::string OpCorporaIO::writePhemInfo(analyze::WordFormPtr wform) const
{
    std::ostringstream oss;
    const std::vector<base::PhemTag>& pheminfo = wform->getPhemInfo();
    const utils::UniString word_form = wform->getWordForm();
    if (pheminfo.empty())
    {
        if (!word_form.isEmpty())
            throw std::runtime_error("PhemInfo is not parsed for word " + word_form.getRawString());
        return "";
    }
    oss << word_form.charAtAsString(0);
    base::PhemTag prev = pheminfo[0];
    for (size_t i = 1; i < word_form.length(); ++i)
    {
        if (!tagsEqual(prev, pheminfo[i]))
            oss << ":" << printPhemTag(prev) << "/";
        prev = pheminfo[i];
        oss << word_form.charAtAsString(i);
    }

    oss << ":" << printPhemTag(prev);

    return oss.str();
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

