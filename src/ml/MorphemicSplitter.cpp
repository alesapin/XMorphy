#include <ml/MorphemicSplitter.h>
#include <tag/PhemTag.h>
#include <unordered_map>
#include <tag/TokenTypeTag.h>
#include <incbin.h>


namespace ml
{

namespace
{
    INCBIN(morphemmodel, "models/morphem.json");
}

MorphemicSplitter::MorphemicSplitter()
    : sequence_size(36) {
    std::istringstream model_is(std::string{reinterpret_cast<const char*>(gmorphemmodelData), gmorphemmodelSize});

    model = std::make_unique<KerasModel>(model_is);
}

static const std::unordered_map<char16_t, size_t> LETTERS = {
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

static std::unordered_set<char16_t> VOWELS = {
    u'А', u'а', u'ё', u'Ё', u'О', u'о', u'Е', u'е',
    u'и', u'И', u'У', u'у', u'Ы', u'ы', u'Э', u'э',
    u'Ю', u'ю', u'Я', u'я',
};

[[maybe_unused]] static void dumpVector(const std::vector<float>& vec, size_t seq_size, const utils::UniString& word) {
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

void MorphemicSplitter::fillLetterFeatures(
    std::vector<float> & to_fill,
    size_t start_pos,
    const utils::UniString& word,
    size_t letter_pos) const
{
    auto upper = X::toupper(word[letter_pos]);
    size_t letter_index = LETTERS.at(upper);
    to_fill[start_pos] = VOWELS.count(upper);
    to_fill[start_pos + letter_index + 1] = 1.0;
}

std::vector<float> MorphemicSplitter::convertWordToVector(const utils::UniString& word) const {
    static const auto one_letter_size = LETTERS.size() + 1 + 1;
    //std::cerr << "ONE LETTER SIZE:" << one_letter_size << std::endl;
    std::vector<float> result(one_letter_size * sequence_size, 0.0);
    //std::cerr << "TOTAL INPUT SIZE:" << result.size() << std::endl;
    size_t start_pos = 0;

    for(size_t i = 0; i < word.length(); ++i)
    {
        fillLetterFeatures(result, start_pos, word, i);
        start_pos += one_letter_size;
    }
    //dumpVector(result, sequence_size, word);

    return result;
}

std::vector<base::PhemTag> MorphemicSplitter::parsePhemInfo(const fdeep::tensor& tensor, size_t word_length, const utils::UniString & word) const
{
    static constexpr auto WORD_PARTS_SIZE = 11;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    size_t step = WORD_PARTS_SIZE;
    size_t i = 0;

    //dumpVector(std::vector<float>(begin, end), WORD_PARTS_SIZE, word);
    //std::cerr << "RESULT LENGTH:" << end - begin << std::endl;
    std::vector<base::PhemTag> result;
    for (auto it = begin; it != end && i < word_length; it += step, ++i) {
        auto max_pos = std::max_element(it, it + step);
        size_t max_index = std::distance(it, max_pos);
        //std::cerr << "MAX INDEX:" << max_index << std::endl;
        result.push_back(base::PhemTag::get(max_index));
    }
    return result;
}

void MorphemicSplitter::split(analyze::WordFormPtr form) const {
    if (form->getType() & base::TokenTypeTag::WORD) {
        const utils::UniString& word_form = form->getWordForm();
        std::vector<float> features = convertWordToVector(word_form);
        fdeep::tensors vector_res = model->predict(std::move(features));
        auto phem_info = parsePhemInfo(vector_res[0], word_form.length(), word_form);
        form->setPhemInfo(phem_info);
    }
}

}
