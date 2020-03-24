#pragma once

#include <fasttext.h>
#include <morph/WordForm.h>
#include <utils/UniString.h>

namespace ml
{

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

    std::vector<float> getWordVector(const utils::UniString & word) const;

    std::vector<float> getWordVector(const analyze::WordFormPtr & word_form) const;
};

}
