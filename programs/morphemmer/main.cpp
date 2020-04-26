#include <codecvt>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <incbin.h>
#include <fdeep/fdeep.hpp>

/// Part of functionality from MorphemicSplitter but independet from main
/// library
INCBIN(model, "models/morphem_20_word_len.json");

using size_t = std::size_t;

static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> UTF16CONV;

static const std::unordered_map<char16_t, size_t> LETTERS = {
    {u'О', 1},  {u'о', 1},  {u'Е', 2},  {u'е', 2},  {u'а', 3},  {u'А', 3},  {u'И', 4},  {u'и', 4},  {u'Н', 5},  {u'н', 5},
    {u'Т', 6},  {u'т', 6},  {u'С', 7},  {u'с', 7},  {u'Р', 8},  {u'р', 8},  {u'В', 9},  {u'в', 9},  {u'Л', 10}, {u'л', 10},
    {u'К', 11}, {u'к', 11}, {u'М', 12}, {u'м', 12}, {u'Д', 13}, {u'д', 13}, {u'П', 14}, {u'п', 14}, {u'У', 15}, {u'у', 15},
    {u'Я', 16}, {u'я', 16}, {u'Ы', 17}, {u'ы', 17}, {u'Ь', 18}, {u'ь', 18}, {u'Г', 19}, {u'г', 19}, {u'З', 20}, {u'з', 20},
    {u'Б', 21}, {u'б', 21}, {u'Ч', 22}, {u'ч', 22}, {u'Й', 23}, {u'й', 23}, {u'Х', 24}, {u'х', 24}, {u'Ж', 25}, {u'ж', 25},
    {u'Ш', 26}, {u'ш', 26}, {u'Ю', 27}, {u'ю', 27}, {u'Ц', 28}, {u'ц', 28}, {u'Щ', 29}, {u'щ', 29}, {u'Э', 30}, {u'э', 30},
    {u'Ф', 31}, {u'ф', 31}, {u'Ъ', 32}, {u'ъ', 32}, {u'Ё', 33}, {u'ё', 33}, {u'-', 34},
};

static std::unordered_set<char16_t> VOWELS = {
    u'А', u'а', u'ё', u'Ё', u'О', u'о', u'Е', u'е', u'и', u'И', u'У', u'у', u'Ы', u'ы', u'Э', u'э', u'Ю', u'ю', u'Я', u'я',
};

enum MorphTag
{
    UNKN = 0,
    PREF = 1,
    ROOT = 2,
    SUFF = 3,
    END = 4,
    LINK = 5,
    HYPH = 6,
    POSTFIX = 7,
    B_SUFF = 8,
    B_PREF = 9,
    B_ROOT = 10,
};

static const char * MORPH_TAGS[] = {
    "UNKN",
    "PREF",
    "ROOT",
    "SUFF",
    "END",
    "LINK",
    "HYPH",
    "POSTFIX",
    "SUFF", /// B-SUFF
    "PREF", /// B-PREF
    "ROOT", /// B-ROOT
};

bool fillLetterFeatures(std::vector<float> & to_fill, size_t start_pos, const std::u16string_view & word, size_t letter_pos)
{
    auto it = LETTERS.find(word[letter_pos]);
    if (it == LETTERS.end())
        return false;
    size_t letter_index = it->second;
    to_fill[start_pos] = VOWELS.count(word[letter_pos]);
    to_fill[start_pos + letter_index + 1] = 1.0;
    return true;
}

std::optional<std::vector<float>> convertWordToVector(const std::u16string_view & word, size_t sequence_size)
{
    static constexpr auto one_letter_size = 36;
    std::vector<float> result(one_letter_size * sequence_size, 0.0);
    size_t start_pos = 0;

    for (size_t i = 0; i < word.length(); ++i)
    {
        if (!fillLetterFeatures(result, start_pos, word, i))
            return {};
        start_pos += one_letter_size;
    }
    return result;
}

std::vector<MorphTag> parsePhemInfo(const fdeep::tensor & tensor, size_t word_length)
{
    static constexpr auto WORD_PARTS_SIZE = 11;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    size_t step = WORD_PARTS_SIZE;
    size_t i = 0;

    std::vector<MorphTag> result(word_length);
    for (auto it = begin; it != end && i < word_length; it += step, ++i)
    {
        auto max_pos = std::max_element(it, it + step);
        size_t max_index = std::distance(it, max_pos);
        result[i] = static_cast<MorphTag>(max_index);
    }
    return result;
}

std::vector<MorphTag> split(const std::u16string_view & word, size_t sequence_size, const fdeep::model & model)
{
    static const auto one_letter_size = 36;

    std::vector<std::u16string_view> input;
    size_t tail_diff = 0;
    if (word.size() > sequence_size)
    {
        size_t i;
        for (i = 0; i < word.size() - sequence_size; i += sequence_size)
            input.push_back(std::u16string_view{word.begin() + i, sequence_size});

        if (i != word.size())
        {
            tail_diff = word.size() - i;
            input.push_back(std::u16string_view{word.end() - sequence_size, sequence_size});
        }
    }
    else
    {
        input.push_back(word);
    }

    std::vector<MorphTag> result(word.length(), MorphTag::UNKN);
    size_t result_index = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        auto features = convertWordToVector(input[i], sequence_size);
        if (!features)
            return result;

        fdeep::tensors vector_res
            = model.predict({fdeep::tensor(fdeep::tensor_shape(sequence_size, one_letter_size), std::move(*features))});
        auto subword_tags = parsePhemInfo(vector_res[0], input[i].length());
        if (i != input.size() - 1 || tail_diff == 0)
            for (size_t j = 0; j < subword_tags.size(); ++result_index, ++j)
                result[result_index] = subword_tags[j];
        else
            for (size_t j = tail_diff; j > 0; --j)
                result[result.size() - j] = subword_tags[subword_tags.size() - j];
    }
    return result;
}

static bool tagsEqual(MorphTag left, MorphTag right)
{
    if (left == MorphTag::B_PREF && right == MorphTag::B_PREF)
        return false;
    if (left == MorphTag::B_ROOT && right == MorphTag::B_ROOT)
        return false;
    if (left == MorphTag::B_SUFF && right == MorphTag::B_SUFF)
        return false;

    if (left == MorphTag::B_PREF && right == MorphTag::PREF)
        return true;
    if (left == MorphTag::B_ROOT && right == MorphTag::ROOT)
        return true;
    if (left == MorphTag::B_SUFF && right == MorphTag::SUFF)
        return true;

    return left == right;
}

std::string printResult(const std::u16string & word, const std::vector<MorphTag> & phem_info)
{
    std::string result;
    result.reserve(word.size() * 2 + word.size() / 3 * 4);
    result.append(UTF16CONV.to_bytes(word[0]));
    MorphTag prev = phem_info[0];

    for (size_t i = 1; i < word.length(); ++i)
    {
        if (!tagsEqual(prev, phem_info[i]))
        {
            result.append(":");
            result.append(MORPH_TAGS[static_cast<size_t>(prev)]);
            result.append("/");
        }
        prev = phem_info[i];
        result.append(UTF16CONV.to_bytes(word[i]));
    }

    result.append(":");
    result.append(MORPH_TAGS[static_cast<size_t>(prev)]);

    return result;
}

std::string processSingleWord(const std::string & s, const fdeep::model & model, size_t sequence_size)
{
    auto word = UTF16CONV.from_bytes(s);
    auto phem_info = split(word, sequence_size, model);
    return printResult(word, phem_info);
}

void processData(std::istream & in, size_t sequence_size, const fdeep::model & model, size_t parallel)
{
    auto func = [&model, &sequence_size](const std::string & s) { return processSingleWord(s, model, sequence_size); };

    while (!in.eof())
    {
        std::vector<std::string> batch;
        while (!in.eof())
        {
            std::string current;
            std::getline(in, current);
            if (current.empty())
                continue;
            batch.push_back(current);
            if (batch.size() == parallel)
                break;
        }
        std::vector<std::future<std::string>> results;
        for (size_t i = 0; i < batch.size(); ++i)
        {
            results.push_back(std::async(func, batch[i]));
        }

        for (size_t i = 0; i < results.size(); ++i)
        {
            std::cout << batch[i] << '\t' << results[i].get() << std::endl;
        }
    }
}


int main(int argc, char ** argv)
{
    std::string model_data{reinterpret_cast<const char *>(gmodelData), gmodelSize};
    auto model_stream = std::istringstream(model_data);
    fdeep::model model(fdeep::read_model(model_stream, false, nullptr));
    if (argc == 2)
    {
        std::ifstream ifs(argv[1]);
        processData(ifs, 20, model, 4);
    }
    else
    {
        processData(std::cin, 20, model, 4);
    }
    return 0;
}
