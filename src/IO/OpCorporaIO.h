#ifndef _OP_CORPORA_H
#define _OP_CORPORA_H
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <morph/WordForm.h>
namespace X
{
class OpCorporaIO
{
public:
    std::string write(WordFormPtr wform) const;
    boost::property_tree::ptree writeToJSON(WordFormPtr wform) const;
    std::string writePhemInfo(WordFormPtr wform) const;
};
}
#endif
