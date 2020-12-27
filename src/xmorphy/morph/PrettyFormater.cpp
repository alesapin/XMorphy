#include <xmorphy/morph/PrettyFormater.h>
#include <tabulate/table.hpp>
#include <xmorphy/morph/WordFormPrinter.h>

namespace X
{

tabulate::Table printTabular(tabulate::Table & word_form_table, const WordFormPtr & form, bool with_phem)
{
    using Row = std::vector<variant<std::string, const char *, tabulate::Table>>;
    for (const auto & mi : form->getMorphInfo())
    {
        Row mi_row;
        mi_row.push_back(form->getWordForm().getRawString());
        mi_row.push_back(mi.normalForm.toLowerCase().getRawString());
        mi_row.push_back(to_string(mi.sp));
        mi_row.push_back(to_string(mi.tag));
        mi_row.push_back(to_string(mi.at));
        mi_row.push_back(std::to_string(mi.probability));
        if (with_phem)
        {
            if (!form->getPhemInfo().empty())
                mi_row.push_back(WordFormPrinter::writePhemInfo(form));
            else
                mi_row.push_back("");
        }
        word_form_table.add_row(mi_row);
    }
    return word_form_table;
}

std::string PrettyFormater::format(const std::vector<WordFormPtr> & forms) const
{
    using namespace tabulate;
    using Row = std::vector<variant<std::string, const char *, Table>>;
    Row header = {"Form", "Normal form", "Speech Part", "Morphotags", "Source", "Probability"};
    if (with_morphemic_info)
        header.push_back("Morphemic parse");

    Table sentence_table;
    sentence_table.add_row(header);

    for (size_t i = 0; i < header.size(); ++i)
    {
        sentence_table[0][i].format()
            .font_align(FontAlign::center)
            .font_style({FontStyle::bold});
    }
    for (auto & ptr : forms)
        if (!(ptr->getTokenType() & TokenTypeTag::SEPR))
            printTabular(sentence_table, ptr, with_morphemic_info);

    sentence_table.column(0).format()
        .multi_byte_characters(true);

    sentence_table.column(1).format()
        .multi_byte_characters(true);

    if (with_morphemic_info)
        sentence_table.column(header.size() - 1).format()
            .multi_byte_characters(true);

    return sentence_table.str();
}
}
