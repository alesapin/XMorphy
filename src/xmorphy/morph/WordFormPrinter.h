#pragma once
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <xmorphy/morph/WordForm.h>
namespace X
{
class WordFormPrinter
{
public:
    static std::string write(WordFormPtr wform);
    static boost::property_tree::ptree writeToJSON(WordFormPtr wform);
    static std::string writePhemInfo(WordFormPtr wform);
    static std::string writePhemInfo(const UniString & word_form, const std::vector<PhemTag> & pheminfo);
};
}
