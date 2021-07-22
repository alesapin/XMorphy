#include <xmorphy/ml/JoinedModel.h>

#include <incbin.h>
#include <codecvt>

namespace X
{

namespace
{
    INCBIN(joinedmodel, "models/joined_model_fine_tuned.json");
}

INCBIN_EXTERN(embeddings);
INCBIN_EXTERN(morphemdict);

JoinedModel::JoinedModel()
    : sentence_size(9)
    , word_size(20)
{
    std::istringstream embeddings_is(std::string{reinterpret_cast<const char *>(gembeddingsData), gembeddingsSize});
    std::istringstream joinedmodel_is(std::string{reinterpret_cast<const char *>(gjoinedmodelData), gjoinedmodelSize});
    std::istringstream phemdict_is(std::string{reinterpret_cast<const char *>(gmorphemdictData), gmorphemdictSize});

    embedding = std::make_unique<Embedding>(embeddings_is);
    model = std::make_unique<KerasModel>(joinedmodel_is);
    phem_dict = PhemDict::loadFromFiles(phemdict_is);
}


void JoinedModel::fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        size_t index = UniSPTag::get(info.sp);
        data[start + index] = 1;
    }
}

void JoinedModel::fillCaseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void JoinedModel::fillNumberFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void JoinedModel::fillGenderFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void JoinedModel::fillTenseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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


namespace
{

    const std::unordered_map<char16_t, size_t> LETTERS = {
        {u'О', 1},  {u'о', 1},  {u'Е', 2},  {u'е', 2},  {u'а', 3},  {u'А', 3},  {u'И', 4},  {u'и', 4},  {u'Н', 5},  {u'н', 5},
        {u'Т', 6},  {u'т', 6},  {u'С', 7},  {u'с', 7},  {u'Р', 8},  {u'р', 8},  {u'В', 9},  {u'в', 9},  {u'Л', 10}, {u'л', 10},
        {u'К', 11}, {u'к', 11}, {u'М', 12}, {u'м', 12}, {u'Д', 13}, {u'д', 13}, {u'П', 14}, {u'п', 14}, {u'У', 15}, {u'у', 15},
        {u'Я', 16}, {u'я', 16}, {u'Ы', 17}, {u'ы', 17}, {u'Ь', 18}, {u'ь', 18}, {u'Г', 19}, {u'г', 19}, {u'З', 20}, {u'з', 20},
        {u'Б', 21}, {u'б', 21}, {u'Ч', 22}, {u'ч', 22}, {u'Й', 23}, {u'й', 23}, {u'Х', 24}, {u'х', 24}, {u'Ж', 25}, {u'ж', 25},
        {u'Ш', 26}, {u'ш', 26}, {u'Ю', 27}, {u'ю', 27}, {u'Ц', 28}, {u'ц', 28}, {u'Щ', 29}, {u'щ', 29}, {u'Э', 30}, {u'э', 30},
        {u'Ф', 31}, {u'ф', 31}, {u'Ъ', 32}, {u'ъ', 32}, {u'Ё', 33}, {u'ё', 33}, {u'-', 34},
    };

    std::unordered_set<char16_t> VOWELS = {
        u'А', u'а', u'ё', u'Ё', u'О', u'о', u'Е', u'е', u'и', u'И', u'У', u'у', u'Ы', u'ы', u'Э', u'э', u'Ю', u'ю', u'Я', u'я',
    };

    std::unordered_set<char16_t> NUMBERS = {
        u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9',
    };

    struct Pair
    {
        size_t start;
        size_t length;
    };

    void fillLetterFeatures(std::vector<float> & to_fill, size_t start_pos, const UniString & word, size_t letter_pos)
    {
        auto it = LETTERS.find(word[letter_pos]);
        size_t letter_index;
        if (it == LETTERS.end())
        {
            if (NUMBERS.count(word[letter_pos]))
                letter_index = 35;
            else
                letter_index = 0;
        }
        else
        {
            letter_index = it->second;
        }
        to_fill[start_pos] = VOWELS.count(word[letter_pos]);
        to_fill[start_pos + letter_index + 1] = 1.0;
    }

    std::vector<float> convertWordToVector(const UniString & word, size_t sequence_size)
    {
        static constexpr auto letter_feature_size = 37;
        std::vector<float> result(letter_feature_size * sequence_size, 0.0);
        size_t start_pos = 0;

        for (size_t i = 0; i < word.length(); ++i)
        {
            fillLetterFeatures(result, start_pos, word, i);
            start_pos += letter_feature_size;
        }
        return result;
    }

    std::vector<PhemTag> parsePhemInfo(const fdeep::tensor & tensor, size_t word_length)
    {
        static constexpr auto WORD_PARTS_SIZE = 12;
        auto begin = tensor.as_vector()->begin();
        auto end = tensor.as_vector()->end();
        size_t step = WORD_PARTS_SIZE;
        size_t i = 0;

        std::vector<PhemTag> result(word_length);
        for (auto it = begin; it != end && i < word_length; it += step, ++i)
        {
            auto max_pos = std::max_element(it, it + step);
            size_t max_index = std::distance(it, max_pos);
            result[i] = PhemTag::get(max_index);
        }
        return result;
    }
}


void JoinedModel::getSpeechPartsFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
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

void JoinedModel::getCaseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
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

void JoinedModel::getNumberFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
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

void JoinedModel::getGenderFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
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

void JoinedModel::getTenseFromTensor(const fdeep::tensor & tensor, std::vector<MorphInfo> & results) const
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

Sentence JoinedModel::filterTokens(const Sentence & input) const
{
    Sentence result;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i]->getTokenType() != TokenTypeTag::SEPR && input[i]->getTokenType() != TokenTypeTag::HIER)
            result.push_back(input[i]);
    }
    return result;
}

std::vector<Sentence> JoinedModel::splitSentenceToBatches(const Sentence & input) const
{
    //std::cerr << "Input size:" << input.size() << std::endl;
    std::vector<Sentence> result;
    auto tail = std::vector<WordFormPtr>(input.end() - sentence_size, input.end());
    for (size_t i = 0; i + sentence_size < input.size(); i += sentence_size)
    {
        auto start = input.begin() + i;
        result.push_back(Sentence(start, start + sentence_size));
    }
    result.push_back(tail);
    return result;
}


void JoinedModel::disambiguateAndMorphemicSplitImpl(Sentence & forms) const
{
    static constexpr auto morpho_features_size = UniSPTag::size() + UniMorphTag::caseSize() + 1 + UniMorphTag::numberSize() + 1
        + UniMorphTag::genderSize() + 1 + UniMorphTag::tenseSize() + 1;

    const size_t one_input_size = embedding->getVectorSize() + morpho_features_size;
    const size_t sequence_input_size = one_input_size * sentence_size;
    std::vector<float> features(sequence_input_size, 0);
    size_t current_size = 0;
    std::vector<float> morphemic_features;
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

        const auto & word = wf->getWordForm();
        if (word.length() < word_size)
        {
            auto word_features = convertWordToVector(word, word_size);
            morphemic_features.insert(morphemic_features.end(), word_features.begin(), word_features.end());
        }
        else
        {
            morphemic_features.insert(morphemic_features.end(), 0, word_size);
        }

    }
    if (morphemic_features.size() < word_size * sentence_size * 37)
        morphemic_features.resize(word_size * sentence_size * 37);

    auto vector_res = model->predictTwoInputs(std::move(features), std::move(morphemic_features));
    //std::cerr << "vector res size:" << vector_res.size() << std::endl;

    std::vector<MorphInfo> result;
    result.resize(forms.size());

    getSpeechPartsFromTensor(vector_res[0], result);
    getCaseFromTensor(vector_res[1], result);
    getNumberFromTensor(vector_res[2], result);
    getGenderFromTensor(vector_res[3], result);
    getTenseFromTensor(vector_res[4], result);

    processFormsWithResultInfos(forms, result);
    auto parts = fdeep::internal::tensor_to_tensors_height_slices(vector_res[5]);
    //std::cerr << "Parts size:" << parts.size() << std::endl;
    for (size_t i = 0; i < forms.size(); ++i)
    {
        if (forms[i]->getWordForm().length() < word_size)
        {
            auto tags = parsePhemInfo(parts[i], forms[i]->getWordForm().length());
        //std::cerr << "Got tags:" << tags.size() << " for:" << forms[i]->getWordForm() << std::endl;
            forms[i]->setPhemInfo(tags);
        }
    }
    //std::cerr << "Depth:" << vector_res[5].depth() << std::endl;
    //std::cerr << "Width:" << vector_res[5].width() << std::endl;
    //std::cerr << "Height:" << vector_res[5].height() << std::endl;
}

size_t JoinedModel::countIntersection(UniMorphTag target, UniMorphTag candidate) const
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

namespace
{
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

void JoinedModel::processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const
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

void JoinedModel::disambiguateAndMorphemicSplit(Sentence & forms) const
{
    auto filtered_forms = filterTokens(forms);
    if (filtered_forms.size() == 0)
        return;

    if (filtered_forms.size() < sentence_size)
    {
        disambiguateAndMorphemicSplitImpl(filtered_forms);

    }
    else
    {
        auto sentence_parts = splitSentenceToBatches(filtered_forms);
        std::vector<std::vector<MorphInfo>> infos;
        for (size_t i = 0; i < sentence_parts.size(); ++i)
            disambiguateAndMorphemicSplitImpl(sentence_parts[i]);
    }
}

}
