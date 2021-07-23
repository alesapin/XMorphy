#include <xmorphy/morph/WordForm.h>

namespace X
{
bool MorphInfo::operator<(const MorphInfo & other) const
{
    return std::tie(normalForm, tag, sp) < std::tie(other.normalForm, other.tag, other.sp);
}

bool MorphInfo::operator==(const MorphInfo & other) const
{
    return std::tie(normalForm, tag, sp) == std::tie(other.normalForm, other.tag, other.sp);
}

size_t WordForm::getInfoHash() const
{
    size_t result = 0;
    UniSPTag all_sps;
    UniMorphTag all_cases, all_numbers, all_genders, all_tenses;
    AnalyzerTag analyzer_tags;
    for (const auto & info : morphInfos)
    {
        all_sps |= info.sp;
        all_cases |= info.tag.getCase();
        all_numbers |= info.tag.getNumber();
        all_genders |= info.tag.getGender();
        all_tenses |= info.tag.getTense();
        analyzer_tags |= info.at;
    }

    boost::hash_combine(result, std::hash<ITag>{}(all_sps));
    boost::hash_combine(result, std::hash<ITag>{}(all_cases));
    boost::hash_combine(result, std::hash<ITag>{}(all_numbers));
    boost::hash_combine(result, std::hash<ITag>{}(all_genders));
    boost::hash_combine(result, std::hash<ITag>{}(all_tenses));
    boost::hash_combine(result, std::hash<ITag>{}(analyzer_tags));
    return result;
}

size_t getApproxSentenceHash(const Sentence & sentence)
{
    size_t result = 0;
    for (const auto & form : sentence)
        boost::hash_combine(result, form->getInfoHash());
    return result;
}

}
