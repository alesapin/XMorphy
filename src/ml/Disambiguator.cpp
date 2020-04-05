#include <ml/Disambiguator.h>

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
    //std::cerr << "CASE FOR:" << form->getWordForm() << std::endl;
    for (const auto& info : form->getMorphInfo())
    {
        //std::cerr << "Info case:" << info.tag.getCase() << std::endl;
        auto info_case = info.tag.getCase();
        if (info_case == base::UniMorphTag::UNKN)
        {
            //std::cerr << "Filling index:" << start << std::endl;;
            data[start] = 1;
        }
        else {
            size_t index = base::UniMorphTag::getCase(info_case);
            //std::cerr << "Case index:" << index << std::endl;
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
    //std::cerr << "SP tensor size:" << end - begin << std::endl;
    for (auto it = begin; it != end && i < results.size(); it += step) {
        //for (size_t j = 0; j < step; ++j)
        //{
        //    std::cerr << std::fixed << *(it + j) << " ";
        //}
        //std::cerr << "\n";
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step) * i;
        //std::cerr << "MAX INDEX:" << max_index << std::endl;
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
    //std::cerr << "Case tensor size:" << end - begin << std::endl;
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

void Disambiguator::disambiguate(analyze::Sentence& forms) const {
    if (forms.size() == 0)
        return;

    static constexpr auto morpho_features_size = base::UniSPTag::size() + base::UniMorphTag::caseSize() + 1 + base::UniMorphTag::numberSize() + 1 + base::UniMorphTag::genderSize() + 1 + base::UniMorphTag::tenseSize() + 1;

    std::vector<analyze::MorphInfo> result;
    result.resize(forms.size());

    const size_t one_input_size = embedding.getVectorSize() + morpho_features_size;
    const size_t sequence_input_size = one_input_size * sequence_size;
    std::vector<float> features(sequence_input_size, 0);

    size_t current_size = 0;
    for (const auto& wf : forms) {
        //std::cerr << "WORD: " << wf->getWordForm() << "\n";
        auto em = embedding.getWordVector(wf);
        std::copy_n(em.data(), em.size(), features.begin() + current_size);
        //std::cerr << "VECTOR:\n";

        //for (size_t j = 0; j < em.size(); ++j) {
        //    std::cerr << features[current_size + j] << " ";
        //}
        //std::cerr << std::endl;

        current_size += em.size();

        fillSpeechPartFeature(wf, features, current_size);
        //std::cerr << "SPEECH PART\n";
        //for (size_t j = 0; j < base::UniSPTag::size(); ++j)
        //{
        //    std::cerr << features[current_size + j] << " ";
        //}
        //std::cerr << std::endl;

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

    fdeep::tensors vector_res = model.predictSingle(std::move(features));

    //std::cerr <<  "TENSORS SIZE:" << vector_res.size() << std::endl;
    getSpeechPartsFromTensor(vector_res[0], result);
    getCaseFromTensor(vector_res[1], result);
    getNumberFromTensor(vector_res[2], result);
    getGenderFromTensor(vector_res[3], result);
    getTenseFromTensor(vector_res[4], result);

    for (size_t i = 0; i < forms.size(); ++i)
    {
        auto & morph_infos = forms[i]->getMorphInfo();
        auto result_morph_info = result[i];
        std::optional<analyze::MorphInfo> most_probable_mi;
        double current_probability = 0;
        for (auto it = morph_infos.begin(); it != morph_infos.end();)
        {
            if (it->probability > current_probability)
            {
                most_probable_mi = *it;
                current_probability = it->probability;
            }

            if (it->sp != result_morph_info.sp
                || it->tag.getCase() != result_morph_info.tag.getCase()
                || it->tag.getGender() != result_morph_info.tag.getGender()
                || it->tag.getNumber() != result_morph_info.tag.getNumber()
                || it->tag.getTense() != result_morph_info.tag.getTense())
            {
                it = morph_infos.erase(it);
            }
            else
                ++it;
        }
        if (morph_infos.empty())
        {
            result_morph_info.normalForm = most_probable_mi->normalForm;
            result_morph_info.stemLen = most_probable_mi->stemLen;
            result_morph_info.probability = 1.0;
            morph_infos.insert(result_morph_info);
        }
    }
}
} // namespace ml
