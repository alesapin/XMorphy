#pragma once

#include <fasttext.h>
#include <morph/WordForm.h>
#include <utils/UniString.h>

namespace X
{

using WordVector = std::vector<float>;

class Embedding
{
private:
    fasttext::FastText ft_embeddings;
public:
    Embedding(std::istream & is);

    size_t getVectorSize() const
    {
        return ft_embeddings.getDimension();
    }

    fasttext::Vector getWordVector(const utils::UniString & word) const;

    fasttext::Vector getWordVector(const WordFormPtr & word_form) const;
};

}
