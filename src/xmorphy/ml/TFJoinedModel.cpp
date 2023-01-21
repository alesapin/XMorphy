#include <xmorphy/ml/TFJoinedModel.h>

#include <incbin.h>
#include <codecvt>

namespace X
{

namespace
{
    INCBIN(tf_joinedmodel_12_20, "models/joined_tflite_model1628685196_new_12_20.tflite");
    INCBIN(tf_joinedmodel_9_20, "models/joined_tflite_model1628348661_new_9_20.tflite");
    //INCBIN(tf_joinedmodel_9_20, "models/joined_tflite_model1628681146_new_9_20_int8_full.tflite");
    //INCBIN(tf_joinedmodel_9_20, "models/joined_tflite_model1628676973_new_9_20_fp16.tflite");
    INCBIN(tf_joinedmodel_9_12, "models/joined_tflite_model1628505134_new_9_12.tflite");
    INCBIN(tf_joinedmodel_9_6, "models/joined_tflite_model1628506970_new_9_6.tflite");
    INCBIN(tf_joinedmodel_7_6, "models/joined_tflite_model1628441900_new_7_6.tflite");
    INCBIN(tf_joinedmodel_7_12, "models/joined_tflite_model1628439989_new_7_12.tflite");
    INCBIN(tf_joinedmodel_7_20, "models/joined_tflite_model1628426746_new_7_20.tflite");
    INCBIN(tf_joinedmodel_5_6, "models/joined_tflite_model1628443261_new_5_6.tflite");
    INCBIN(tf_joinedmodel_5_12, "models/joined_tflite_model1628438339_new_5_12.tflite");
    INCBIN(tf_joinedmodel_5_20, "models/joined_tflite_model1628432800_new_5_20.tflite");
    INCBIN(tf_joinedmodel_3_20, "models/joined_tflite_model1628434895_new_3_20.tflite");
}

INCBIN_EXTERN(embeddings);
INCBIN_EXTERN(morphemdict);

TFJoinedModel::TFJoinedModel()
{
    std::istringstream embeddings_is(std::string{reinterpret_cast<const char *>(gembeddingsData), gembeddingsSize});
    std::istringstream phemdict_is(std::string{reinterpret_cast<const char *>(gmorphemdictData), gmorphemdictSize});

    embedding = std::make_unique<Embedding>(embeddings_is);

    auto model_12_20 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_12_20Data), gtf_joinedmodel_12_20Size);
    auto model_9_20 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_9_20Data), gtf_joinedmodel_9_20Size);
    auto model_9_12 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_9_12Data), gtf_joinedmodel_9_12Size);
    auto model_9_6 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_9_6Data), gtf_joinedmodel_9_6Size);
    auto model_7_20 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_7_20Data), gtf_joinedmodel_7_20Size);
    auto model_7_12 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_7_12Data), gtf_joinedmodel_7_12Size);
    auto model_7_6 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_7_6Data), gtf_joinedmodel_7_6Size);
    auto model_5_20 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_5_20Data), gtf_joinedmodel_5_20Size);
    auto model_5_12 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_5_12Data), gtf_joinedmodel_5_12Size);
    auto model_5_6 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_5_6Data), gtf_joinedmodel_5_6Size);
    auto model_3_20 = std::make_shared<TensorflowModel>(reinterpret_cast<const char *>(gtf_joinedmodel_3_20Data), gtf_joinedmodel_3_20Size);

    std::map<size_t, TensorflowModelPtr> models_3 = {
        {20, model_3_20}
    };
    std::map<size_t, TensorflowModelPtr> models_5 = {
        {20, model_5_20},
        {12, model_5_12},
        {6, model_5_6},
    };
    std::map<size_t, TensorflowModelPtr> models_7 = {
        {20, model_7_20},
        {12, model_7_12},
        {6, model_7_6},
    };
    std::map<size_t, TensorflowModelPtr> models_9 = {
        {20, model_9_20},
        {12, model_9_12},
        {6, model_9_6},
    };
    std::map<size_t, TensorflowModelPtr> models_12 = {
        {20, model_12_20},
    };

    std::map<size_t, std::map<size_t, TensorflowModelPtr>> predictors;
    predictors[3] = models_3;
    predictors[5] = models_5;
    predictors[7] = models_7;
    predictors[9] = models_9;
    predictors[12] = models_12;

    model = std::make_unique<TensorflowMultiModel2d>(std::move(predictors));

    phem_dict = PhemDict::loadFromFiles(phemdict_is);
}

std::vector<Sentence> TFJoinedModel::splitSentenceToBatches(const Sentence & input, size_t sentence_size) const
{
    std::vector<Sentence> result;
    size_t total_sentences = input.size() / sentence_size;

    for (size_t i = 0; i + sentence_size < input.size(); i += sentence_size)
    {
        auto start = input.begin() + i;
        result.push_back(Sentence(start, start + sentence_size));
    }

    size_t tail_length = input.size() - total_sentences * sentence_size;
    if (tail_length != 0)
    {
        size_t rounded_tail = model->getRoundedSizeFirst(tail_length);
        Sentence tail(input.end() - rounded_tail, input.end());
        result.push_back(tail);
    }

    return result;
}

void TFJoinedModel::fillSpeechPartFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
{
    for (const auto & info : form->getMorphInfo())
    {
        size_t index = UniSPTag::get(info.sp);
        data[start + index] = 1;
    }
}

void TFJoinedModel::fillCaseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void TFJoinedModel::fillNumberFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void TFJoinedModel::fillGenderFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

void TFJoinedModel::fillTenseFeature(const WordFormPtr form, std::vector<float> & data, size_t start) const
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

    [[maybe_unused]] std::vector<PhemTag> parsePhemInfo(const NonOwningTensor2d<float> & tensor, size_t word_length)
    {
        std::vector<PhemTag> result(word_length);
        for (size_t i = 0; i < word_length; ++i)
        {
            size_t max_index = tensor.argmax(i);
            result[i] = PhemTag::get(max_index);
        }
        return result;
    }


}

static const std::pair<size_t, size_t> MODEL_NOT_FIT = std::pair<size_t, size_t>(0, 0);

std::pair<size_t, size_t> TFJoinedModel::selectModelForSentence(const Sentence & forms) const
{
    size_t max_word_length = 0;
    for (const auto & form : forms)
        max_word_length = std::max(form->getWordForm().length(), max_word_length);

    if (forms.size() > model->getModelMaxSizeFirst())
        return MODEL_NOT_FIT;

    size_t size_first = model->getRoundedSizeFirst(forms.size());

    if (model->getModelMaxSizeSecond(size_first) < max_word_length)
        return MODEL_NOT_FIT;

    return model->getRoundedSize(size_first, max_word_length);
}


void TFJoinedModel::getSpeechPartsFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const
{
    for (size_t i = 0; i < results.size(); ++i)
    {
        auto max_index = tensor.argmax(i);
        results[i].sp = UniSPTag::get(max_index);
    }
}

void TFJoinedModel::getCaseFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const
{
    for (size_t i = 0; i < results.size(); ++i)
    {
        auto max_index = tensor.argmax(i);
        if (max_index != 0)
            results[i].tag.setCase(UniMorphTag::getCase(max_index - 1));
    }
}

void TFJoinedModel::getNumberFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const
{
    for (size_t i = 0; i < results.size(); ++i)
    {
        auto max_index = tensor.argmax(i);
        if (max_index != 0)
            results[i].tag.setNumber(UniMorphTag::getNum(max_index - 1));
    }
}

void TFJoinedModel::getGenderFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const
{
    for (size_t i = 0; i < results.size(); ++i)
    {
        auto max_index = tensor.argmax(i);
        if (max_index != 0)
            results[i].tag.setGender(UniMorphTag::getGen(max_index - 1));
    }
}

void TFJoinedModel::getTenseFromTensor(const NonOwningTensor2d<float> & tensor, std::vector<MorphInfo> & results) const
{
    for (size_t i = 0; i < results.size(); ++i)
    {
        auto argmax = tensor.argmax(i);
        if (argmax != 0)
            results[i].tag.setTense(UniMorphTag::getTense(argmax - 1));
    }
}

Sentence TFJoinedModel::filterTokens(const Sentence & input) const
{
    Sentence result;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (input[i]->getTokenType() != TokenTypeTag::SEPR && input[i]->getTokenType() != TokenTypeTag::HIER)
            result.push_back(input[i]);
    }
    return result;
}

bool TFJoinedModel::disambiguateAndMorphemicSplitImpl(Sentence & forms) const
{
    auto suitable_model = selectModelForSentence(forms);
    if (suitable_model == MODEL_NOT_FIT)
        return false;

    auto [sentence_size, word_size] = suitable_model;

    static constexpr auto morpho_features_size = UniSPTag::size() + UniMorphTag::caseSize() + 1 + UniMorphTag::numberSize() + 1
        + UniMorphTag::genderSize() + 1 + UniMorphTag::tenseSize() + 1;

    const size_t one_input_size = embedding->getVectorSize() + morpho_features_size;
    std::vector<float> features(forms.size() * one_input_size, 0);
    size_t current_size = 0;
    std::vector<float> morphemic_features;
    morphemic_features.reserve(forms.size() * word_size * 37);

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
        auto word_features = convertWordToVector(word, word_size);
        morphemic_features.insert(morphemic_features.end(), word_features.begin(), word_features.end());
    }

    //if (morphemic_features.size() < word_size * sentence_size * 37)
    //    morphemic_features.resize(word_size * sentence_size * 37);

    auto vector_res = model->predict(sentence_size, word_size, std::move(features), std::move(morphemic_features));
    //std::cerr << "vector res size:" << vector_res.size() << std::endl;

    std::vector<MorphInfo> result;
    result.resize(forms.size());

    getSpeechPartsFromTensor(vector_res[0], result);
    getCaseFromTensor(vector_res[1], result);
    getNumberFromTensor(vector_res[2], result);
    getGenderFromTensor(vector_res[3], result);
    getTenseFromTensor(vector_res[4], result);

    processFormsWithResultInfos(forms, result);
    size_t morph_start = 5;
    for (size_t i = 0; i < forms.size(); ++i)
    {
        if (forms[i]->getWordForm().length() <= word_size)
        {
            auto tags = parsePhemInfo(vector_res[morph_start + i], forms[i]->getWordForm().length());
            forms[i]->setPhemInfo(tags);
        }
        else
        {
            return false;
        }
    }
    //std::cerr << "Depth:" << vector_res[5].depth() << std::endl;
    //std::cerr << "Width:" << vector_res[5].width() << std::endl;
    //std::cerr << "Height:" << vector_res[5].height() << std::endl;
    return true;
}

size_t TFJoinedModel::countIntersection(UniMorphTag target, UniMorphTag candidate) const
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

void TFJoinedModel::processFormsWithResultInfos(Sentence & forms, const std::vector<MorphInfo> & result_infos) const
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

bool TFJoinedModel::disambiguateAndMorphemicSplit(Sentence & forms) const
{
    auto filtered_forms = filterTokens(forms);
    if (filtered_forms.size() == 0)
        return false;

    if (filtered_forms.size() <= model->getModelMaxSizeFirst())
    {
        return disambiguateAndMorphemicSplitImpl(filtered_forms);
    }
    else
    {
        size_t max_word_length = 1;
        for (const auto & form : filtered_forms)
        {
            //std::cerr << form->getWordForm() << ' ';
            max_word_length = std::max(form->getWordForm().length(), max_word_length);
        }

        if (model->getModelMaxSizeSecond(model->getModelMaxSizeFirst()) < max_word_length)
            return false;

        auto sentence_parts = splitSentenceToBatches(filtered_forms, model->getModelMaxSizeFirst());

        std::vector<std::vector<MorphInfo>> infos;
        for (size_t i = 0; i < sentence_parts.size(); ++i)
        {
            if (!disambiguateAndMorphemicSplitImpl(sentence_parts[i]))
                return false;
        }

        return true;
    }
}

}
