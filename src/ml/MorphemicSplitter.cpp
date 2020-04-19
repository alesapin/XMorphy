#include <ml/MorphemicSplitter.h>
#include <tag/PhemTag.h>
#include <utils/UniCharacter.h>
#include <unordered_map>
#include <tag/TokenTypeTag.h>

namespace ml
{

static const std::unordered_map<utils::UniCharacter, size_t> LETTERS =
{
    {utils::UniCharacter::O, 1},
    {utils::UniCharacter::YE, 2},
    {utils::UniCharacter::A, 3},
    {utils::UniCharacter::I, 4},
    {utils::UniCharacter::N, 5},
    {utils::UniCharacter::T, 6},
    {utils::UniCharacter::S, 7},
    {utils::UniCharacter::R, 8},
    {utils::UniCharacter::V, 9},
    {utils::UniCharacter::L, 10},
    {utils::UniCharacter::K, 11},
    {utils::UniCharacter::M, 12},
    {utils::UniCharacter::D, 13},
    {utils::UniCharacter::P, 14},
    {utils::UniCharacter::U, 15},
    {utils::UniCharacter::YA, 16},
    {utils::UniCharacter::AUY, 17},
    {utils::UniCharacter::MG, 18},
    {utils::UniCharacter::G, 19},
    {utils::UniCharacter::ZE, 20},
    {utils::UniCharacter::B, 21},
    {utils::UniCharacter::CH, 22},
    {utils::UniCharacter::YI, 23},
    {utils::UniCharacter::H, 24},
    {utils::UniCharacter::JE, 25},
    {utils::UniCharacter::SH, 26},
    {utils::UniCharacter::UY, 27},
    {utils::UniCharacter::CE, 28},
    {utils::UniCharacter::SHE, 29},
    {utils::UniCharacter::AE, 30},
    {utils::UniCharacter::F, 31},
    {utils::UniCharacter::TV, 32},
    {utils::UniCharacter::YO, 33},
    {utils::UniCharacter("-"), 34},
};

void MorphemicSplitter::fillLetterFeatures(
    std::vector<float> & to_fill,
    size_t start_pos,
    const utils::UniString& word,
    size_t letter_pos) const {

    //std::cerr << "WORD:" << word << std::endl;
    auto upper = word[letter_pos].toUpper();
    size_t letter_index = LETTERS.at(upper);
    to_fill[start_pos] = utils::UniCharacter::VOWELS.count(upper);
    to_fill[start_pos + letter_index] = 1.0;
}

std::vector<float> MorphemicSplitter::convertWordToVector(const utils::UniString& word) const {
    static const auto one_letter_size = LETTERS.size() + 1 + 1;
    std::vector<float> result(one_letter_size * sequence_size, 0.0);
    size_t start_pos = 0;

    for(size_t i = 0; i < word.length(); ++i)
    {
        fillLetterFeatures(result, start_pos, word, i);
        start_pos += one_letter_size;
    }

    return result;
}

std::vector<base::PhemTag> MorphemicSplitter::parsePhemInfo(const fdeep::tensor& tensor, size_t word_length) const
{
    static constexpr auto WORD_PARTS_SIZE = 11;
    auto begin = tensor.as_vector()->begin();
    auto end = tensor.as_vector()->end();
    size_t step = WORD_PARTS_SIZE;
    size_t i = 0;

    std::vector<base::PhemTag> result;
    for (auto it = begin; it != end && i < word_length; it += step, ++i) {
        auto max_pos = std::max_element(it, it + step);
        size_t max_index = std::distance(begin, max_pos) - (step) * i;
        result.push_back(base::PhemTag::get(max_index));
    }
    return result;
}

void MorphemicSplitter::split(analyze::WordFormPtr form) const {
    if (form->getType() & base::TokenTypeTag::WORD) {
        const utils::UniString& word_form = form->getWordForm();
        std::vector<float> features = convertWordToVector(word_form);
        fdeep::tensors vector_res = model->predict(std::move(features));
        auto phem_info = parsePhemInfo(vector_res[0], word_form.length());
        form->setPhemInfo(phem_info);
    }
}

}
