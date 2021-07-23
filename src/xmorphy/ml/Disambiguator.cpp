#include <xmorphy/ml/Disambiguator.h>

#include <queue>
#include <xmorphy/graphem/Token.h>

#include <incbin.h>

namespace X
{

INCBIN(embeddings, "models/morphorueval_cbow.embedding_50.bin");

namespace
{
    INCBIN(disambmodel3, "models/disamb_layer_3_pooled_3.json");
    INCBIN(disambmodel5, "models/disamb_layer_3_pooled_5.json");
    INCBIN(disambmodel7, "models/disamb_layer_3_pooled_7.json");
    INCBIN(disambmodel9, "models/disamb_layer_3_pooled_9.json");

    [[maybe_unused]] void dumpVector(const std::vector<float> & vec, size_t seq_size)
    {
        std::cerr << "SEQ SIZE:" << seq_size << std::endl;
        for (size_t i = 0; i < vec.size(); i += seq_size)
        {
            for (size_t k = i; k < i + seq_size; ++k)
            {
                std::cerr << vec[k] << " ";
            }
            std::cerr << "\n";
        }
    }

    void setPredictedTags(const MorphInfo & predicted, MorphInfo & from_dict)
    {
        from_dict.sp = predicted.sp;
        const auto & predicted_mt = predicted.tag;
        auto & mt = from_dict.tag;

        if (predicted_mt.hasCase())
            mt.setCase(predicted_mt.getCase());

        if (predicted_mt.hasGender())
            mt.setGender(predicted_mt.getGender());

        if (predicted_mt.hasNumber())
            mt.setNumber(predicted_mt.getNumber());

        if (predicted_mt.hasTense())
            mt.setTense(predicted_mt.getTense());
    }
}

Disambiguator::Disambiguator()
{
    std::istringstream embeddings_is(std::string{reinterpret_cast<const char *>(gembeddingsData), gembeddingsSize});

    std::istringstream disambmodel3_is(std::string{reinterpret_cast<const char *>(gdisambmodel3Data), gdisambmodel3Size});
    std::istringstream disambmodel5_is(std::string{reinterpret_cast<const char *>(gdisambmodel5Data), gdisambmodel5Size});
    std::istringstream disambmodel7_is(std::string{reinterpret_cast<const char *>(gdisambmodel7Data), gdisambmodel7Size});
    std::istringstream disambmodel9_is(std::string{reinterpret_cast<const char *>(gdisambmodel9Data), gdisambmodel9Size});
    embedding = std::make_unique<Embedding>(embeddings_is);

    std::map<size_t, KerasModelPtr> predictors = {
        {3, std::make_shared<KerasModel>(disambmodel3_is)},
        {5, std::make_shared<KerasModel>(disambmodel5_is)},
        {7, std::make_shared<KerasModel>(disambmodel7_is)},
        {9, std::make_shared<KerasModel>(disambmodel9_is)},
    };

    model = std::make_unique<KerasMultiModel>(std::move(predictors));
}

void Disambiguator::fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        size_t index = UniSPTag::get(info.sp);
        data[start + index] = 1;
    }
}

void Disambiguator::fillCaseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        auto info_case = info.tag.getCase();
        if (info_case == UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = UniMorphTag::getCase(info_case);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillNumberFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        auto info_num = info.tag.getNumber();
        if (info_num == UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = UniMorphTag::getNum(info_num);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillGenderFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        auto info_gen = info.tag.getGender();
        if (info_gen == UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = UniMorphTag::getGen(info_gen);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::fillTenseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        auto info_tense = info.tag.getTense();
        if (info_tense == UniMorphTag::UNKN)
        {
            data[start] = 1;
        }
        else
        {
            size_t index = UniMorphTag::getTense(info_tense);
            data[start + index + 1] = 1;
        }
    }
}

void Disambiguator::getSpeechPartsFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
{
    size_t i = 0;
    size_t step = UniSPTag::size();
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step)
    {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step) * i;
        results[i].sp = UniSPTag::get(max_index);
        ++i;
    }
}

void Disambiguator::getCaseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
{
    size_t i = 0;
    size_t step = UniMorphTag::caseSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step)
    {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setCase(UniMorphTag::getCase(max_index - 1));
        ++i;
    }
}

void Disambiguator::getNumberFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
{
    size_t i = 0;
    size_t step = UniMorphTag::numberSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step)
    {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setNumber(UniMorphTag::getNum(max_index - 1));
        ++i;
    }
}

void Disambiguator::getGenderFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
{
    size_t i = 0;
    size_t step = UniMorphTag::genderSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step)
    {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setGender(UniMorphTag::getGen(max_index - 1));
        ++i;
    }
}

void Disambiguator::getTenseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
{
    size_t i = 0;
    size_t step = UniMorphTag::tenseSize() + 1;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    for (auto it = begin; it != end && i < results.size(); it += step)
    {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(begin, max_pos) - (step)*i;
        if (max_index != 0)
            results[i].tag.setTense(UniMorphTag::getTense(max_index - 1));
        ++i;
    }
}


std::vector<Sentence> Disambiguator::splitSentenceToBatches(const Sentence & input) const
{
    size_t sequence_size = model->getModelMaxSize();
    std::vector<Sentence> result;
    auto tail = std::vector<WordFormPtr>(input.end() - sequence_size, input.end());
    for (size_t i = 0; i + sequence_size < input.size(); i += sequence_size)
    {
        auto start = input.begin() + i;
        result.push_back(Sentence(start, start + sequence_size));
    }
    result.push_back(tail);
    return result;
}

std::vector<MorphInfo> Disambiguator::disambiguateImpl(const Sentence & forms, size_t sequence_size) const
{
    static constexpr auto morpho_features_size = UniSPTag::size() + UniMorphTag::caseSize() + 1 + UniMorphTag::numberSize() + 1
        + UniMorphTag::genderSize() + 1 + UniMorphTag::tenseSize() + 1;

    const size_t one_input_size = embedding->getVectorSize() + morpho_features_size;
    const size_t sequence_input_size = one_input_size * sequence_size;
    std::vector<float> features(sequence_input_size, 0);

    size_t current_size = 0;
    for (const auto & wf : forms)
    {
        auto em = embedding->getWordVector(wf);
        std::copy_n(em.data(), em.size(), features.begin() + current_size);

        current_size += em.size();
        fillSpeechPartFeature(wf, features, current_size);
        current_size += UniSPTag::size();

        fillCaseFeature(wf, features, current_size);
        current_size += UniMorphTag::caseSize() + 1;

        fillNumberFeature(wf, features, current_size);
        current_size += UniMorphTag::numberSize() + 1;

        fillGenderFeature(wf, features, current_size);
        current_size += UniMorphTag::genderSize() + 1;

        fillTenseFeature(wf, features, current_size);
        current_size += UniMorphTag::tenseSize() + 1;
    }
    fdeep::tensors vector_res = model->predict(sequence_size, std::move(features));

    std::vector<MorphInfo> result;
    result.resize(forms.size());

    getSpeechPartsFromTensor(vector_res[0], result);
    getCaseFromTensor(vector_res[1], result);
    getNumberFromTensor(vector_res[2], result);
    getGenderFromTensor(vector_res[3], result);
    getTenseFromTensor(vector_res[4], result);

    return result;
}

size_t Disambiguator::countIntersection(UniMorphTag target, UniMorphTag candidate) const
{
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

void Disambiguator::processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const
{
    for (size_t i = 0; i < forms.size(); ++i)
    {
        std::unordered_set<MorphInfo> & morph_infos = forms[i]->getMorphInfo();
        if (morph_infos.empty())
            throw std::runtime_error("No morph info found for form " + forms[i]->getWordForm().getRawString());
        auto deduced_morph_info = result_infos[i];

        double current_dict = 0;
        std::optional<MorphInfo> most_probable_dict;

        std::map<size_t, std::vector<MorphInfo>> ordered_mi;

        for (auto it = morph_infos.begin(); it != morph_infos.end(); ++it)
        {
            if (it->at == AnalyzerTag::DICT && it->probability > current_dict)
            {
                most_probable_dict = *it;
                current_dict = it->probability;
            }

            if (it->sp != deduced_morph_info.sp)
                continue;

            size_t intersection = countIntersection(deduced_morph_info.tag, it->tag);
            intersection += (it->at == AnalyzerTag::DICT);
            ordered_mi[intersection].push_back(*it);
        }

        if (most_probable_dict && ((most_probable_dict->probability > 0.7 && UniSPTag::getStaticSPs().count(most_probable_dict->sp) != 0)
                || most_probable_dict->probability > 0.9))
        {
            most_probable_dict->probability = 1.;
            setPredictedTags(deduced_morph_info, *most_probable_dict);
            forms[i]->setMorphInfo({*most_probable_dict});
        }
        else if (ordered_mi.empty())
        {
            MorphInfo max = *morph_infos.begin();
            for (const auto & mi : morph_infos)
            {
                if (mi.probability > max.probability)
                    max = mi;
            }
            max.probability = 1.;
            setPredictedTags(deduced_morph_info, max);
            forms[i]->setMorphInfo({max});
        }
        else
        {
            for (auto it = ordered_mi.rbegin(); it != ordered_mi.rend(); ++it)
            {
                auto max = std::max_element(
                    it->second.begin(), it->second.end(), [](const auto & l, const auto & r) { return l.probability < r.probability; });

                setPredictedTags(deduced_morph_info, *max);
                max->probability = 1.;
                forms[i]->setMorphInfo({*max});
                break;
            }
        }
    }
}

Sentence Disambiguator::filterTokens(const Sentence & input, std::vector<bool> & mask) const
{
    Sentence result;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i]->getTokenType() != TokenTypeTag::SEPR && input[i]->getTokenType() != TokenTypeTag::HIER)
            result.push_back(input[i]);
        else
            mask[i] = true;
    }
    return result;
}

void Disambiguator::disambiguate(Sentence & input_forms) const
{
    std::vector<bool> mask(input_forms.size());
    auto forms = filterTokens(input_forms, mask);
    if (forms.size() == 0)
        return;

    if (forms.size() <= model->getModelMaxSize())
    {
        auto rounded_size = model->roundSequenceSize(forms.size());
        auto result = disambiguateImpl(forms, rounded_size);
        processFormsWithResultInfos(forms, result);
    }
    else
    {
        auto sentence_parts = splitSentenceToBatches(forms);
        std::vector<std::vector<MorphInfo>> infos;
        for (size_t i = 0; i < sentence_parts.size(); ++i)
        {
            auto rounded_size = model->roundSequenceSize(sentence_parts[i].size());
            auto result = disambiguateImpl(sentence_parts[i], rounded_size);
            infos.push_back(result);
        }

        for (size_t i = 0; i < sentence_parts.size(); ++i)
        {
            processFormsWithResultInfos(sentence_parts[i], infos[i]);
        }
    }
}

}
