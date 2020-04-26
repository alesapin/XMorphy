#include <ml/MorphemicSplitter.h>
#include <tag/PhemTag.h>
#include <unordered_map>
#include <tag/TokenTypeTag.h>
#include <incbin.h>


namespace X
{

namespace
{
    INCBIN(morphemmodel, "models/morphem_20_word_len.json");
}

MorphemicSplitter::MorphemicSplitter()
    : sequence_size(20) {
    std::istringstream model_is(std::string{reinterpret_cast<const char*>(gmorphemmodelData), gmorphemmodelSize});

    model = std::make_unique<KerasModel>(model_is);
}

namespace {
const std::unordered_map<char16_t, size_t> LETTERS = {
    {u'О', 1},
    {u'о', 1},
    {u'Е', 2},
    {u'е', 2},
    {u'а', 3},
    {u'А', 3},
    {u'И', 4},
    {u'и', 4},
    {u'Н', 5},
    {u'н', 5},
    {u'Т', 6},
    {u'т', 6},
    {u'С', 7},
    {u'с', 7},
    {u'Р', 8},
    {u'р', 8},
    {u'В', 9},
    {u'в', 9},
    {u'Л', 10},
    {u'л', 10},
    {u'К', 11},
    {u'к', 11},
    {u'М', 12},
    {u'м', 12},
    {u'Д', 13},
    {u'д', 13},
    {u'П', 14},
    {u'п', 14},
    {u'У', 15},
    {u'у', 15},
    {u'Я', 16},
    {u'я', 16},
    {u'Ы', 17},
    {u'ы', 17},
    {u'Ь', 18},
    {u'ь', 18},
    {u'Г', 19},
    {u'г', 19},
    {u'З', 20},
    {u'з', 20},
    {u'Б', 21},
    {u'б', 21},
    {u'Ч', 22},
    {u'ч', 22},
    {u'Й', 23},
    {u'й', 23},
    {u'Х', 24},
    {u'х', 24},
    {u'Ж', 25},
    {u'ж', 25},
    {u'Ш', 26},
    {u'ш', 26},
    {u'Ю', 27},
    {u'ю', 27},
    {u'Ц', 28},
    {u'ц', 28},
    {u'Щ', 29},
    {u'щ', 29},
    {u'Э', 30},
    {u'э', 30},
    {u'Ф', 31},
    {u'ф', 31},
    {u'Ъ', 32},
    {u'ъ', 32},
    {u'Ё', 33},
    {u'ё', 33},
    {u'-', 34},
};

std::unordered_set<char16_t> VOWELS = {
    u'А',
    u'а',
    u'ё',
    u'Ё',
    u'О',
    u'о',
    u'Е',
    u'е',
    u'и',
    u'И',
    u'У',
    u'у',
    u'Ы',
    u'ы',
    u'Э',
    u'э',
    u'Ю',
    u'ю',
    u'Я',
    u'я',
};

[[maybe_unused]] void dumpVector(const std::vector<float>& vec, size_t seq_size, const utils::UniString& word) {
    size_t j = 0;
    for(size_t i = 0; i < vec.size() && j < word.length(); i+=seq_size, ++j)
    {
        std::cerr << "LETTER:" << word[j]<< " ";
        for (size_t j = i; j < i + seq_size; ++j)
        {
            std::cerr << vec[j] << " ";
        }
        std::cerr << "\n";
    }
}

struct Pair
{
    size_t start;
    size_t length;
};

bool fillLetterFeatures(
    std::vector<float>& to_fill,
    size_t start_pos,
    const utils::UniString& word,
    size_t letter_pos) {
    auto it = LETTERS.find(word[letter_pos]);
    if (it == LETTERS.end())
        return false;
    size_t letter_index = it->second;
    to_fill[start_pos] = VOWELS.count(word[letter_pos]);
    to_fill[start_pos + letter_index + 1] = 1.0;
    return true;
}

std::optional<std::vector<float>> convertWordToVector(Pair pair, const utils::UniString & word, size_t sequence_size) {
    static constexpr auto one_letter_size = 36;
    std::vector<float> result(one_letter_size * sequence_size, 0.0);
    size_t start_pos = 0;

    for (size_t i = pair.start; i < pair.start + pair.length; ++i) {
        if (!fillLetterFeatures(result, start_pos, word, i))
            return {};
        start_pos += one_letter_size;
    }
    return result;
}

std::vector<PhemTag> parsePhemInfo(const fdeep::tensor& tensor, size_t word_length) {
    static constexpr auto WORD_PARTS_SIZE = 11;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    size_t step = WORD_PARTS_SIZE;
    size_t i = 0;

    std::vector<PhemTag> result(word_length);
    for (auto it = begin; it != end && i < word_length; it += step, ++i) {
        auto max_pos = std::max_element(it, it + step);
        size_t max_index = std::distance(it, max_pos);
        result[i] = PhemTag::get(max_index);
    }
    return result;
}

} // namespace

std::vector<PhemTag> MorphemicSplitter::split(const utils::UniString& word) const {
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
        auto features = convertWordToVector(input[i], word, sequence_size);
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

void MorphemicSplitter::split(WordFormPtr form) const {
    if (form->getType() & TokenTypeTag::WORD) {
        const utils::UniString& word_form = form->getWordForm();
        auto result = split(word_form);
        form->setPhemInfo(result);
    }
}

}
