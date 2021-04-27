#pragma once

#include <fasttext.h>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/utils/UniString.h>

namespace X
{
using WordVector = std::vector<float>;

class Embedding
{
private:
    fasttext::FastText ft_embeddings;

public:
    Embedding(std::istream & is);

    size_t getVectorSize() const { return ft_embeddings.getDimension(); }

    fasttext::Vector getWordVector(const UniString & word) const;

    fasttext::Vector getWordVector(const WordFormPtr & word_form) const;
};

}
