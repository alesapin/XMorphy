#include <xmorphy/morph/TSVFormater.h>

#include <xmorphy/morph/WordFormPrinter.h>

namespace X
{

std::string TSVFormater::format(const std::vector<WordFormPtr> & forms) const
{
    std::ostringstream oss;
    for (const auto & form : forms)
    {
        if (form->getTokenType() & TokenTypeTag::SEPR)
            continue;
        for (const auto & mi : form->getMorphInfo())
        {
            oss << form->getWordForm().getRawString() << "\t";
            oss << mi.normalForm.toLowerCase().getRawString() << "\t";
            oss << mi.sp << "\t";
            oss << mi.tag << "\t";
            oss << mi.at << "\t";
            oss << mi.probability;
            const std::vector<PhemTag> & pheminfo = form->getPhemInfo();
            if (with_morphemic_info && !pheminfo.empty())
            {
                oss << "\t";
                oss << WordFormPrinter::writePhemInfo(form);
            }
            oss << "\n";
        }
    }
    auto result = oss.str();
    result.pop_back();
    return result;
}

}
