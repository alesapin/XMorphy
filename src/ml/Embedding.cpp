#include <ml/Embedding.h>
namespace ml
{
Embedding::Embedding(std::istream & is)
{
    ft_embeddings.loadModel(is);
}

std::vector<float> Embedding::getWordVector(const utils::UniString& word) const
{
    /// Ugly copy
    fasttext::Vector v(getVectorSize());
    ft_embeddings.getWordVector(v, word.getRawString());
    std::vector<float> result(v.data(), v.data() + v.size());
    return result;
}

std::vector<float> Embedding::getWordVector(const analyze::WordFormPtr& word_form) const
{
    return getWordVector(word_form->getWordForm());
}
}
