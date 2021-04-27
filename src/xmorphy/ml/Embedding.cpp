#include <xmorphy/ml/Embedding.h>
namespace X
{
Embedding::Embedding(std::istream & is)
{
    int32_t magic;
    int32_t version;
    /// Just skip two check number
    is.read((char *)&(magic), sizeof(int32_t));
    is.read((char *)&(version), sizeof(int32_t));
    ft_embeddings.loadModel(is);
}

fasttext::Vector Embedding::getWordVector(const UniString & word) const
{
    fasttext::Vector v(getVectorSize());
    ft_embeddings.getWordVector(v, word.getRawString());
    return v;
}

fasttext::Vector Embedding::getWordVector(const WordFormPtr & word_form) const
{
    return getWordVector(word_form->getWordForm());
}
}
