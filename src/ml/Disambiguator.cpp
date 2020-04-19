#include <ml/Disambiguator.h>
#include <graphem/Token.h>
#include <queue>

namespace ml {

void Disambiguator::fillSpeechPartFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const
{
    for (const auto& info : form->getMorphInfo())
    {
        size_t index = base::UniSPTag::get(info.sp);
        data[start + index] = 1;
    }
}

void Disambiguator::fillCaseFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const
{
    for (const auto& info : form->getMorphInfo())
    {
        auto info_case = info.tag.getCase();
        if (info_case == base::UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = base::UniMorphTag::getCase(info_case);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillNumberFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const
{
    for (const auto& info : form->getMorphInfo())
    {
        auto info_num = info.tag.getNumber();
        if (info_num == base::UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = base::UniMorphTag::getNum(info_num);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillGenderFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const
{
    for (const auto& info : form->getMorphInfo())
    {
        auto info_gen = info.tag.getGender();
        if (info_gen == base::UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = base::UniMorphTag::getGen(info_gen);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillTenseFeature(const analyze::WordFormPtr form, std::vector<float>& data, size_t start) const
{
    for (const auto& info : form->getMorphInfo())
    {
        auto info_tense = info.tag.getTense();
        if (info_tense == base::UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = base::UniMorphTag::getTense(info_tense);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::getSpeechPartsFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const
{
    size_t i = 0;
    size_t step = base::UniSPTag::size();
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step) * i;
        results[i].sp = base::UniSPTag::get(max_index);
        ++i;
    }
}

void Disambiguator::getCaseFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const
{
    size_t i = 0;
    size_t step = base::UniMorphTag::caseSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step) * i;
        if (max_index != 0)
            results[i].tag.setCase(base::UniMorphTag::getCase(max_index - 1));
        ++i;
    }
}

void Disambiguator::getNumberFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const
{
    size_t i = 0;
    size_t step = base::UniMorphTag::numberSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setNumber(base::UniMorphTag::getNum(max_index - 1));
        ++i;
    }
}

void Disambiguator::getGenderFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const
{
    size_t i = 0;
    size_t step = base::UniMorphTag::genderSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setGender(base::UniMorphTag::getGen(max_index - 1));
        ++i;
    }
}

void Disambiguator::getTenseFromTensor(const fdeep::tensor& tensor, std::vector<analyze::MorphInfo>& results) const
{
    size_t i = 0;
    size_t step = base::UniMorphTag::tenseSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setTense(base::UniMorphTag::getTense(max_index - 1));
        ++i;
    }
}


std::vector<analyze::Sentence> Disambiguator::splitSentenceToBatches(const analyze::Sentence & input) const
{
    std::vector<analyze::Sentence> result;
    auto tail = std::vector<analyze::WordFormPtr>(input.end() - sequence_size, input.end());
    for (size_t i = 0; i + sequence_size < input.size(); i += sequence_size)
    {
        auto start = input.begin() + i;
        result.push_back(analyze::Sentence(start, start + sequence_size));
    }
    result.push_back(tail);
    return result;
}

std::vector<analyze::MorphInfo> Disambiguator::disambiguateImpl(analyze::Sentence& forms) const {

    static constexpr auto morpho_features_size = base::UniSPTag::size() + base::UniMorphTag::caseSize() + 1 + base::UniMorphTag::numberSize() + 1 + base::UniMorphTag::genderSize() + 1 + base::UniMorphTag::tenseSize() + 1;

    const size_t one_input_size = embedding->getVectorSize() + morpho_features_size;
    const size_t sequence_input_size = one_input_size * sequence_size;
    std::vector<float> features(sequence_input_size, 0);

    size_t current_size = 0;
    for (const auto& wf : forms) {
        auto em = embedding->getWordVector(wf);
        std::copy_n(em.data(), em.size(), features.begin() + current_size);

        current_size += em.size();
        fillSpeechPartFeature(wf, features, current_size);
        current_size += base::UniSPTag::size();

        fillCaseFeature(wf, features, current_size);
        current_size += base::UniMorphTag::caseSize() + 1;

        fillNumberFeature(wf, features, current_size);
        current_size += base::UniMorphTag::numberSize() + 1;

        fillGenderFeature(wf, features, current_size);
        current_size += base::UniMorphTag::genderSize() + 1;

        fillTenseFeature(wf, features, current_size);
        current_size += base::UniMorphTag::tenseSize() + 1;
    }

    fdeep::tensors vector_res = model->predict(std::move(features));

    std::vector<analyze::MorphInfo> result;
    result.resize(forms.size());

    getSpeechPartsFromTensor(vector_res[0], result);
    getCaseFromTensor(vector_res[1], result);
    getNumberFromTensor(vector_res[2], result);
    getGenderFromTensor(vector_res[3], result);
    getTenseFromTensor(vector_res[4], result);

    return result;
}

size_t Disambiguator::smartCountIntersection(base::UniMorphTag target, base::UniMorphTag candidate) const{
    size_t result = 0;
    if (target.getNumber() == candidate.getNumber())
        result += 4;
    if (target.getGender() == candidate.getGender())
        result += 3;
    if (target.getTense() == candidate.getTense())
        result += 2;
    if (target.getCase() == candidate.getCase())
        result += 1;

    return result;
}

void Disambiguator::processFormsWithResultInfos(analyze::Sentence& forms, const std::vector<analyze::MorphInfo>& result_infos) const
{
    for (size_t i = 0; i < forms.size(); ++i)
    {
        auto & morph_infos = forms[i]->getMorphInfo();
        auto deduced_morph_info = result_infos[i];
        double current_dict = 0;
        std::optional<analyze::MorphInfo> most_probable_dict;

        std::map<size_t, std::vector<analyze::MorphInfo>> ordered_mi;


        for (auto it = morph_infos.begin(); it != morph_infos.end(); ++it)
        {
            if (it->at == base::AnalyzerTag::DICT && it->probability > current_dict) {
                most_probable_dict = *it;
                current_dict = it->probability;
            }

            if (it->sp != deduced_morph_info.sp)
                continue;

            size_t intersection = smartCountIntersection(deduced_morph_info.tag, it->tag);
            intersection += (it->at == base::AnalyzerTag::DICT);
            ordered_mi[intersection].push_back(*it);
        }

        if ((most_probable_dict->probability > 0.7 && base::UniSPTag::getStaticSPs().count(most_probable_dict->sp) != 0) || most_probable_dict->probability > 0.9)
        {
            most_probable_dict->probability = 1.;
            forms[i]->setMorphInfo({*most_probable_dict});
        }
        else if (ordered_mi.empty())
        {
            auto max = std::max_element(morph_infos.begin(), morph_infos.end(), [](const auto& l, const auto& r) { return l.probability < r.probability; });
            max->probability = 1.;
            forms[i]->setMorphInfo({*max});
        }
        else
        {
            for (auto it = ordered_mi.rbegin(); it != ordered_mi.rend(); ++it)
            {
                auto max = std::max_element(it->second.begin(), it->second.end(), [](const auto& l, const auto& r) { return l.probability < r.probability; });
                max->probability = 1.;
                forms[i]->setMorphInfo({*max});
                break;
            }
        }
    }

}

analyze::Sentence Disambiguator::filterTokens(const analyze::Sentence& input, std::vector<bool> & mask) const
{
    analyze::Sentence result;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i]->getType() != base::TokenTypeTag::SEPR && input[i]->getType() != base::TokenTypeTag::HIER)
            result.push_back(input[i]);
        else
            mask[i] = true;
    }
    return result;
}

void Disambiguator::disambiguate(analyze::Sentence& input_forms) const
{
    std::vector<bool> mask(input_forms.size());
    auto forms = filterTokens(input_forms, mask);
    if (forms.size() == 0)
        return;

    if (forms.size() < sequence_size)
    {
        auto result = disambiguateImpl(forms);
        processFormsWithResultInfos(forms, result);
    }
    else
    {
        auto sentence_parts = splitSentenceToBatches(forms);
        std::vector<std::vector<analyze::MorphInfo>> infos;
        for (size_t i = 0; i < sentence_parts.size(); ++i)
            infos.push_back(disambiguateImpl(sentence_parts[i]));

        for (size_t i = 0; i < sentence_parts.size(); ++i)
            processFormsWithResultInfos(sentence_parts[i], infos[i]);
    }

}

} // namespace ml
