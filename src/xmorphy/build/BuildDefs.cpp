#include <xmorphy/build/BuildDefs.h>
namespace X
{
void saveParas(const std::vector<EncodedParadigm> & paraMap, std::ostream & os)
{
    os << paraMap.size() << "\n";
    for (auto epara : paraMap)
    {
        os << epara.size() << "\n";
        for (std::size_t i = 0; i < epara.size(); ++i)
        {
            os << epara[i].prefixId << "\t" << epara[i].tagId << "\t" << epara[i].suffixId << "\t" << epara[i].isNormalForm << "\n";
        }
    }
}

void loadParas(std::vector<EncodedParadigm> & paras, std::istream & is)
{
    std::size_t mapSize, counter = 0;
    std::string row;
    std::getline(is, row);
    mapSize = stoul(row);
    paras.resize(mapSize);
    while (counter < mapSize)
    {
        std::size_t paraSize;
        std::getline(is, row);
        paraSize = stoul(row);
        EncodedParadigm para(paraSize);
        for (std::size_t i = 0; i < paraSize; ++i)
        {
            std::size_t prefixId, tagId, suffixId;
            std::getline(is, row);
            std::vector<std::string> nums;
            boost::split(nums, row, boost::is_any_of("\t"));
            prefixId = stoul(nums[0]);
            tagId = stoul(nums[1]);
            suffixId = stoul(nums[2]);
            bool isNormalForm = (nums[3] == "1");
            para[i] = EncodedLexemeGroup{
                .prefixId = prefixId,
                .tagId = tagId,
                .suffixId = suffixId,
                .isNormalForm = isNormalForm
            };
        }
        paras[counter] = para;
        counter++;
    }
}
}
