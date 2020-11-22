#include <xmorphy/morph/JSONEachSentenceFormater.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <xmorphy/morph/WordFormPrinter.h>

namespace X
{

std::string JSONEachSentenceFormater::format(const std::vector<WordFormPtr> & forms) const
{
    namespace pt = boost::property_tree;
    pt::ptree sentence_array;
    size_t form_num = 0;
    for (const auto & form : forms)
    {
        if (form->getTokenType() & TokenTypeTag::SEPR)
            continue;
        pt::ptree word_form;
        for (auto & mi : form->getMorphInfo())
        {
            pt::ptree elem;
            elem.put("lemma", mi.normalForm.toLowerCase().getRawString());
            elem.put("speech_part", mi.sp);
            elem.put("case", mi.tag.getCase());
            elem.put("gender", mi.tag.getGender());
            elem.put("number", mi.tag.getNumber());
            elem.put("tense", mi.tag.getTense());
            elem.put("raw_tags", mi.tag);
            if (with_morphemic_info && !form->getPhemInfo().empty())
                elem.put("phem_info", WordFormPrinter::writePhemInfo(form));
            word_form.push_back(std::make_pair("", elem));
        }
        sentence_array.push_back(std::make_pair(std::to_string(form_num) + "_" + form->getWordForm().getRawString(), word_form));
        form_num++;
    }

    std::stringstream ss;
    pt::json_parser::write_json(ss, sentence_array);
    return ss.str();
}
}
