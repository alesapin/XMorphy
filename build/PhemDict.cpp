#include "PhemDict.h"
namespace build {

const std::string PhemDict::MAIN_PHEM = "_m";
const std::string PhemDict::FORWARD_PHEM = "_f";
const std::string PhemDict::BACKWARD_PHEM = "_b";

bool PhemDict::contains(const utils::UniString& word) const {
    return dict->contains(word.toUpperCase().getRawString());
}

std::vector<base::PhemTag> PhemDict::getPhemParse(const utils::UniString& word) const {
    PhemMarkup markUp = dict->getValue(word.toUpperCase().getRawString());
    std::vector<base::PhemTag> result;
    for (std::size_t i = 0; i < result.size(); ++i) {
        result.push_back(markUp.getTag(i));
    }
    return result;
}

void dropToFiles(const std::unique_ptr<PhemDict>& dct, const std::string& filename) {
    std::ofstream mofs(filename + PhemDict::MAIN_PHEM);
    std::ofstream fofs(filename + PhemDict::FORWARD_PHEM);
    std::ofstream bofs(filename + PhemDict::BACKWARD_PHEM);
    dct->dict->serialize(mofs);
    dct->fdict->serialize(fofs);
    dct->bdict->serialize(bofs);
}

void loadFromFiles(std::unique_ptr<PhemDict>& dict, const std::string& filename) {
    std::ifstream mifs(filename + PhemDict::MAIN_PHEM);
    std::ifstream fifs(filename + PhemDict::FORWARD_PHEM);
    std::ifstream bifs(filename + PhemDict::BACKWARD_PHEM);

    InnerPhemDictPtr dct = std::make_shared<dawg::Dictionary<PhemMarkup>>();
    dct->deserialize(mifs);
    InnerCounterPhemDictPtr fdct = std::make_shared<dawg::Dictionary<std::size_t>>();
    fdct->deserialize(fifs);
    InnerCounterPhemDictPtr bdct = std::make_shared<dawg::Dictionary<std::size_t>>();
    bdct->deserialize(bifs);
    dict = utils::make_unique<PhemDict>(dct, fdct, bdct);
}
}
