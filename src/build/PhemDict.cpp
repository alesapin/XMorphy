#include "PhemDict.h"
namespace X
{
const std::string PhemDict::MAIN_PHEM = "_m";
const std::string PhemDict::FORWARD_PHEM = "_f";
const std::string PhemDict::BACKWARD_PHEM = "_b";

bool PhemDict::contains(const utils::UniString & word) const
{
    return dict->contains(word.toUpperCase().getRawString());
}

std::vector<PhemTag> PhemDict::getPhemParse(const utils::UniString & word) const
{
    PhemMarkup markUp = dict->getValue(word.toUpperCase().getRawString());
    std::vector<PhemTag> result;
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        result.push_back(markUp.getTag(i));
    }
    return result;
}

void dropToFiles(const std::unique_ptr<PhemDict> & dct, const std::string & filename)
{
    std::ofstream mofs(filename + PhemDict::MAIN_PHEM);
    std::ofstream fofs(filename + PhemDict::FORWARD_PHEM);
    std::ofstream bofs(filename + PhemDict::BACKWARD_PHEM);
    dct->dict->serialize(mofs);
    dct->fdict->serialize(fofs);
    dct->bdict->serialize(bofs);
}

std::unique_ptr<PhemDict> PhemDict::loadFromFiles(std::istream & mainDictIs, std::istream & forwardIs, std::istream & backwardIs)
{
    InnerPhemDictPtr dct = std::make_shared<dawg::Dictionary<PhemMarkup>>();
    dct->deserialize(mainDictIs);
    InnerCounterPhemDictPtr fdct = std::make_shared<dawg::Dictionary<std::size_t>>();
    fdct->deserialize(forwardIs);
    InnerCounterPhemDictPtr bdct = std::make_shared<dawg::Dictionary<std::size_t>>();
    bdct->deserialize(backwardIs);
    return utils::make_unique<PhemDict>(dct, fdct, bdct);
}
}
