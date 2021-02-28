#include <unordered_map>
#include <incbin.h>
#include <codecvt>
#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/tag/PhemTag.h>
#include <xmorphy/tag/TokenTypeTag.h>


using namespace utils;

namespace X
{
namespace
{
    INCBIN(morphemmodel, "models/morphem_model_lemma_only.json");
}

MorphemicSplitter::MorphemicSplitter()
    : sequence_size(20)
{
    std::istringstream model_is(std::string{reinterpret_cast<const char *>(gmorphemmodelData), gmorphemmodelSize});

    model = std::make_unique<KerasModel>(model_is);
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
    }

    ;

    std::unordered_set<char16_t> VOWELS = {
        u'А', u'а', u'ё', u'Ё', u'О', u'о', u'Е', u'е', u'и', u'И', u'У', u'у', u'Ы', u'ы', u'Э', u'э', u'Ю', u'ю', u'Я', u'я',
    };

    [[maybe_unused]] void dumpVector(const std::vector<float> & vec, size_t seq_size, const utils::UniString & word)
    {
        std::cerr << "SEQ SIZE:" << seq_size << std::endl;
        std::cerr << "WORD SIZE:" << word.length() << std::endl;
        static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> UTF16CONV;
        size_t j = 0;
        for (size_t i = 0; i < vec.size() && j < word.length(); i += seq_size, ++j)
        {
            std::cerr << "LETTER:" << UTF16CONV.to_bytes(word[j]) << "\n";

            for (size_t k = i; k < i + seq_size; ++k)
            {
                std::cerr << vec[k] << " ";
            }
            std::cerr << "\n";
        }
    }

    struct Pair
    {
        size_t start;
        size_t length;
    };

    bool fillLetterFeatures(std::vector<float> & to_fill, size_t start_pos, const utils::UniString & word, size_t letter_pos)
    {
        auto it = LETTERS.find(word[letter_pos]);
        if (it == LETTERS.end())
            return false;
        size_t letter_index = it->second;
        to_fill[start_pos] = VOWELS.count(word[letter_pos]);
        to_fill[start_pos + letter_index + 1] = 1.0;
        return true;
    }

    void fillSpeechPartFeature(std::vector<float> & to_fill, size_t start_pos, X::UniSPTag sp)
    {
        to_fill[start_pos + X::UniSPTag::get(sp)] = 1.0;
    }

    std::optional<std::vector<float>> convertWordToVector(Pair pair, const utils::UniString & word, size_t sequence_size, UniSPTag sp)
    {
        static constexpr auto letter_feature_size = 36;
        static constexpr auto one_letter_full_size = letter_feature_size + UniSPTag::size();
        std::vector<float> result(one_letter_full_size * sequence_size, 0.0);
        size_t start_pos = 0;

        for (size_t i = pair.start; i < pair.start + pair.length; ++i)
        {
            if (!fillLetterFeatures(result, start_pos, word, i))
                return {};
            fillSpeechPartFeature(result, start_pos + letter_feature_size, sp);
            start_pos += one_letter_full_size;
        }
        return result;
    }

    std::vector<PhemTag> parsePhemInfo(const fdeep::tensor & tensor, size_t word_length)
    {
        static constexpr auto WORD_PARTS_SIZE = 11;
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

} // namespace

std::vector<PhemTag> MorphemicSplitter::split(const utils::UniString & word, UniSPTag sp) const
{
    std::vector<Pair> input;
    size_t tail_diff = 0;
    if (word.length() > sequence_size)
    {
        size_t i;
        for (i = 0; i < word.length() - sequence_size; i += sequence_size)
            input.emplace_back(Pair{i, sequence_size});

        if (i != word.length())
        {
            tail_diff = word.length() - i;
            input.emplace_back(Pair{word.length() - sequence_size, sequence_size});
        }
    }
    else
    {
        input.emplace_back(Pair{0, word.length()});
    }

    std::vector<PhemTag> result(word.length(), PhemTag::UNKN);
    size_t result_index = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        auto features = convertWordToVector(input[i], word, sequence_size, sp);
        if (!features)
            return result;

        fdeep::tensors vector_res = model->predict(std::move(*features));
        auto subword_tags = parsePhemInfo(vector_res[0], input[i].length);
        if (i != input.size() - 1 || tail_diff == 0)
            for (size_t j = 0; j < subword_tags.size(); ++result_index, ++j)
                result[result_index] = subword_tags[j];
        else
            for (size_t j = tail_diff; j > 0; --j)
                result[result.size() - j] = subword_tags[subword_tags.size() - j];
    }
    return result;
}

void MorphemicSplitter::split(WordFormPtr form) const
{
    if (form->getTokenType() & TokenTypeTag::WORD)
    {
        const utils::UniString & word_form = form->getWordForm();
        auto result = split(word_form, form->getMorphInfo().begin()->sp);
        form->setPhemInfo(result);
    }
}

namespace
{
std::vector<PhemTag> getNounAdjForm(const UniString & orig, const UniString & word, const std::vector<PhemTag> & source_phem)
{
    if (orig == word)
    {
        //if (source_phem.size() != word.length())
        //{
        //    std::cerr << "WORD:" << word << std::endl;
        //    std::cerr << "LEMMA:" << orig << std::endl;
        //    std::cerr << "PARSELEN:" << source_phem.size() << std::endl;
        //}

        return source_phem;
    }

    if (orig.length() > word.length())
    {
        auto result = source_phem;
        result.resize(source_phem.size());
        //if (result.size() != word.length())
        //{
        //    std::cerr << "WORD:" << word << std::endl;
        //    std::cerr << "LEMMA:" << orig << std::endl;
        //    std::cerr << "PARSELEN:" << result.size() << std::endl;
        //}

        return result;
    }

    size_t common_part = 0;
    for (size_t i = 0; i < orig.length(); ++i)
    {
        if (orig[i] == word[i])
            common_part++;
        else
            break;
    }


    if (common_part != source_phem.size())
    {
        auto first_tag = source_phem[common_part];
        if (first_tag != PhemTag::END)
        {
            return {};
        }

        for (size_t i = common_part + 1; i < source_phem.size(); ++i)
        {
            if (source_phem[i] != first_tag)
            {
                return {};
            }
        }
    }


    std::vector<PhemTag> result = source_phem;
    result.insert(result.end(), word.length() - result.size(), PhemTag::END);
    //if (result.size() != word.length())
    //{
    //    std::cerr << "WORD:" << word << std::endl;
    //    std::cerr << "LEMMA:" << orig << std::endl;
    //    std::cerr << "PARSELEN:" << result.size() << std::endl;
    //}
    return result;
}

bool checkNext(const UniString & word, size_t pos, const UniString & expected)
{
    if (pos + expected.length() > word.length())
        return false;
    size_t expected_pos = 0;
    for (size_t i = pos; i < word.length() && expected_pos < expected.length(); ++i, ++expected_pos)
        if (word[i] != expected[expected_pos])
            return false;
    return true;
}

std::vector<PhemTag> getVerbFormV2(const UniString & orig,
    const UniString & word, const std::vector<PhemTag> & source_phem)
{
    if (orig == word)
        return source_phem;

    size_t minlen = std::min(orig.length(), word.length());
    size_t common_part = 0;
    for (size_t i = 0; i < minlen; ++i)
    {
        if (orig[i] == word[i])
            common_part++;
        else
            break;
    }

    /// Root changed and seems like we don't no how to process it
    if (source_phem[common_part] == PhemTag::ROOT)
        return {};

    if (common_part == word.length())
    {
        /// суну сунуть, крикну кринуть
        if (word.endsWith(UniString("НУ")) && orig.endsWith(UniString("НУТЬ")))
            common_part--;
        if (word == UniString("БУДУ"))
            common_part--;
    }

    ///кипячусь кипятится
    if (word.endsWith(UniString("ЧУСЬ")) && orig.endsWith(UniString("ТИТЬСЯ")))
        common_part--;

    /// вскипячу вскипятить
    if (word.endsWith(UniString("ЧУ")) && orig.endsWith(UniString("ТИТЬ")))
        common_part--;

    if (word.endsWith(UniString("ЯТ")) && orig.endsWith(UniString("ЯТЬ"))) // выстоят выстоять
        common_part = word.length() - 2;
    else if (word.endsWith(UniString("ЯТСЯ")) && orig.endsWith(UniString("ЯТЬСЯ")))
        common_part = word.length() - 4;

    if ((word.endsWith(UniString("ИМСЯ"))) && orig.endsWith(UniString("ИТЬСЯ"))) /// учимся учиться
        common_part--;
    else if (word.endsWith(UniString("ИМ")) && orig.endsWith(UniString("ИТЬ")))
        common_part--;

    /// утопимтесь утопиться
    if (word.endsWith(UniString("ИМТЕСЬ")) && orig.endsWith(UniString("ИТЬСЯ")))
        common_part--;

    /// утопимте утопитесь
    if (word.endsWith(UniString("ИМТE")) && orig.endsWith(UniString("ИТЬСЯ")))
        common_part--;

    if (word.endsWith(UniString("ИМТE")) && orig.endsWith(UniString("ИТЬ")))
        common_part--;

    /// раскрошитесь раскрошиться
    if (word.endsWith(UniString("ТЕСЬ")) && orig.endsWith(UniString("ТЬСЯ")) && word.length() - common_part == 3)
        common_part--;

    /// раскрошите раскрошиться
    if (word.endsWith(UniString("ТЕ")) && orig.endsWith(UniString("ТЬСЯ")) && word.length() - common_part == 1)
        common_part--;



    std::vector<PhemTag> result(source_phem.begin(), source_phem.begin() + common_part);

    if (checkNext(word, common_part, UniString("ЁВ")) || (checkNext(word, common_part, UniString("ЕВ")) && !checkNext(word, common_part, UniString("ЕВА")))
        || checkNext(word, common_part, UniString("ВА"))
        || checkNext(word, common_part, UniString("ЕН"))
        || checkNext(word, common_part, UniString("ЕН"))
        || checkNext(word, common_part, UniString("ЯЧ"))
        )
    {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
        common_part += 2;
    }

    if (result.size() == word.length())
        return result;

    if (checkNext(word, common_part, UniString("ОВА"))
        || checkNext(word, common_part, UniString("ЕВА"))
        || checkNext(word, common_part, UniString("ЫВА"))
        || checkNext(word, common_part, UniString("ИВА"))
        || checkNext(word, common_part, UniString("ВШИ")))
    {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 2, PhemTag::SUFF);
        common_part += 3;
    }

    if (result.size() == word.length())
        return result;

    if (common_part == word.length() - 1 && checkNext(word, common_part, UniString("В")))
    {
        result.push_back(PhemTag::B_SUFF);
        common_part += 1;
    }

    if (result.size() == word.length())
        return result;

    else if (word[common_part] == u'Л'
        || word[common_part] == u'Й'
        || (word[common_part] == u'Я' &&
            (!checkNext(word, common_part, UniString("ЯТ")) || checkNext(word, common_part, UniString("ЯТЬ"))))
        || (word[common_part] == u'У' && common_part + 1 != word.length()))
    {
        result.push_back(PhemTag::B_SUFF);
        common_part += 1;
    }

    if (result.size() == word.length())
        return result;

    if (checkNext(word, common_part, UniString("ИТЕ"))
        || checkNext(word, common_part, UniString("ИТЬ")))
     {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 2, PhemTag::SUFF);
        common_part += 3;
    }
    else if (checkNext(word, common_part, UniString("ТЬ"))
        || checkNext(word, common_part, UniString("ТЕ")))
    {

        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
        common_part += 2;
    }

    if (result.size() == word.length())
        return result;

    size_t left_letters = word.length() - common_part;

    if (word.endsWith(UniString("СЯ")) || word.endsWith(UniString("СЬ")))
    {
        if (left_letters > 2)
        {
            if (left_letters >= 4 && checkNext(word, word.length() - 4, UniString("ТЕ")))
            {
                result.insert(result.end(), left_letters - 4, PhemTag::END);
                result.push_back(PhemTag::B_SUFF);
                result.push_back(PhemTag::SUFF);
            }
            else
                result.insert(result.end(), left_letters - 2, PhemTag::END);
        }

        result.insert(result.end(), 2, PhemTag::POSTFIX);
    }
    else if ((word.endsWith(UniString("ТЕ")) && !word.endsWith(UniString("ЕТЕ"))) || word.endsWith(UniString("ТЬ")))
    {
        if (left_letters > 2)
            result.insert(result.end(), left_letters - 2, PhemTag::END);
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
    }
    else
    {
        result.insert(result.end(), left_letters, PhemTag::END);
    }


    return result;
}

}

std::vector<PhemTag> MorphemicSplitter::split(
    const utils::UniString & word, UniSPTag sp,
    const utils::UniString normal_form,
    const std::vector<PhemTag> & normal_form_parse) const
{
    std::vector<PhemTag> result;
    if (sp == UniSPTag::NOUN || sp == UniSPTag::ADJ)
        result = getNounAdjForm(normal_form, word, normal_form_parse);
    else if (sp == UniSPTag::VERB)
        result = getVerbFormV2(normal_form, word, normal_form_parse);
    else
        result = normal_form_parse;

    if (result.empty())
        return result;

    if (result.size() < word.length())
        result.insert(result.end(), word.length() - result.size(), result.back());
    else if (result.size() > word.length())
        result.resize(word.length());

    return result;

}

}
