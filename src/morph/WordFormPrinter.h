#pragma once
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <morph/WordForm.h>
namespace X
{
class WordFormPrinter
{
public:
    std::string write(WordFormPtr wform) const;
    boost::property_tree::ptree writeToJSON(WordFormPtr wform) const;
    std::string writePhemInfo(WordFormPtr wform) const;
    std::string writePhemInfo(const utils::UniString & word_form, const std::vector<PhemTag> & pheminfo) const;
};
}
