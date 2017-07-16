#include "BuildDefs.h"
namespace build {
void saveParas(const std::vector<EncodedParadigm>& paraMap, std::ostream& os) {
    os << paraMap.size() << "\n";
    for (auto epara : paraMap) {
        os << epara.size() << "\n";
        for (std::size_t i = 0; i < epara.size(); ++i) {
            os << std::get<0>(epara[i]) << "\t" << std::get<1>(epara[i]) << "\t" << std::get<2>(epara[i]) << "\n";
        }
    }
}

void loadParas(std::vector<EncodedParadigm>& paras, std::istream& is) {
    std::size_t mapSize, counter = 0;
    std::string row;
    std::getline(is, row);
    mapSize = stoul(row);
    paras.resize(mapSize);
    while (counter < mapSize) {
        std::size_t paraSize;
        std::getline(is, row);
        paraSize = stoul(row);
        EncodedParadigm para(paraSize);
        for (std::size_t i = 0; i < paraSize; ++i) {
            std::size_t prefixId, tagId, suffixId;
            std::getline(is, row);
            std::vector<std::string> nums;
            boost::split(nums, row, boost::is_any_of("\t"));
            prefixId = stoul(nums[0]);
            tagId = stoul(nums[1]);
            suffixId = stoul(nums[2]);
            para[i] = std::make_tuple(prefixId, tagId, suffixId);
        }
        paras[counter] = para;
        counter++;
    }
}
}
