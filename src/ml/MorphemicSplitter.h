#pragma once

#include <morph/WordForm.h>
#include <ml/KerasModel.h>
#include <memory>

namespace ml
{

class MorphemicSplitter
{
private:
    std::unique_ptr<KerasModel> model;
    size_t sequence_size;

    std::vector<base::PhemTag> split(const utils::UniString & word) const;
public:
    MorphemicSplitter(std::istream & model_stream_, size_t sequence_size_)
        : model(std::make_unique<KerasModel>(model_stream_))
        , sequence_size(sequence_size_) {
    }

    MorphemicSplitter();

    void split(analyze::WordFormPtr forms) const;
};

} // namespace ml
