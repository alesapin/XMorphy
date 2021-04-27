#include <xmorphy/morph/WordFormPrinter.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
namespace X
{

std::string WordFormPrinter::write(WordFormPtr wform)
{
    std::ostringstream oss;
    if (wform->getTokenType() & TokenTypeTag::SEPR)
        return "";
    for (const auto & mi : wform->getMorphInfo())
    {
        oss << wform->getWordForm().getRawString() << "\t";
        oss << mi.normalForm.toLowerCase().getRawString() << "\t";
        oss << mi.sp << "\t";
        oss << mi.tag << "\t";
        oss << mi.at << "\t";
        oss << mi.probability;
        const std::vector<PhemTag> & pheminfo = wform->getPhemInfo();
        if (!pheminfo.empty())
        {
            oss << "\t";
            oss << writePhemInfo(wform);
        }
        oss << "\n";
    }
    std::string result = oss.str();
    result.pop_back();
    return result;
}

static std::string printPhemTag(PhemTag tag)
{
    if (tag == PhemTag::B_SUFF)
        return to_string(PhemTag::SUFF);
    if (tag == PhemTag::B_ROOT)
        return to_string(PhemTag::ROOT);
    if (tag == PhemTag::B_PREF)
        return to_string(PhemTag::PREF);
    return to_string(tag);
}

static bool tagsEqual(PhemTag left, PhemTag right)
{
    if (left == PhemTag::B_PREF && right == PhemTag::B_PREF)
        return false;
    if (left == PhemTag::B_ROOT && right == PhemTag::B_ROOT)
        return false;
    if (left == PhemTag::B_SUFF && right == PhemTag::B_SUFF)
        return false;

    if (left == PhemTag::B_PREF && right == PhemTag::PREF)
        return true;
    if (left == PhemTag::B_ROOT && right == PhemTag::ROOT)
        return true;
    if (left == PhemTag::B_SUFF && right == PhemTag::SUFF)
        return true;

    return left == right;
}

std::string WordFormPrinter::writePhemInfo(WordFormPtr wform)
{
    const std::vector<PhemTag> & pheminfo = wform->getPhemInfo();
    const UniString word_form = wform->getWordForm();
    return writePhemInfo(word_form, pheminfo);
}

std::string WordFormPrinter::writePhemInfo(const UniString & word_form, const std::vector<PhemTag> & pheminfo)
{
    if (pheminfo.empty())
    {
        if (!word_form.isEmpty())
            throw std::runtime_error("PhemInfo is not parsed for word " + word_form.getRawString());
        return "";
    }
    if (pheminfo.size() < word_form.length())
        throw std::runtime_error("PhemInfo has length " + std::to_string(pheminfo.size()) + " but word has lengths " + std::to_string(word_form.length()));

    std::ostringstream oss;
    oss << word_form.charAtAsString(0);
    PhemTag prev = pheminfo[0];
    for (size_t i = 1; i < word_form.length(); ++i)
    {
        if (!tagsEqual(prev, pheminfo[i]))
        {
            oss << ":" << printPhemTag(prev) << "/";
        }
        prev = pheminfo[i];
        oss << word_form.charAtAsString(i);
    }

    oss << ":" << printPhemTag(prev);

    return oss.str();
}

boost::property_tree::ptree WordFormPrinter::writeToJSON(WordFormPtr wform)
{
    namespace pt = boost::property_tree;
    pt::ptree array;
    if (wform->getTokenType() & TokenTypeTag::SEPR)
        return array;

    std::ostringstream oss;
    size_t index = 0;
    for (auto & mi : wform->getMorphInfo())
    {
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

}
