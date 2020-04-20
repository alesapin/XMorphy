#pragma once

#include <morph/WordForm.h>
#include <ml/KerasModel.h>
#include <Resource.h>
#include <memory>

namespace ml
{

class MorphemicSplitter
{
private:
    std::unique_ptr<KerasModel> model;
    size_t sequence_size;

    void fillLetterFeatures(std::vector<float>& to_fill, size_t start_pos, const utils::UniString& word, size_t letter_pos) const;
    std::vector<float> convertWordToVector(const utils::UniString& word) const;
    std::vector<base::PhemTag> parsePhemInfo(const fdeep::tensor& tensor, size_t word_length, const utils::UniString & word) const;

    public : MorphemicSplitter(std::istream& model_stream_, size_t sequence_size_)
        : model(std::make_unique<KerasModel>(model_stream_))
        , sequence_size(sequence_size_) {
    }

public:
    MorphemicSplitter()
        : sequence_size(36)
    {
        const auto& factory = CppResource::ResourceFactory::instance();
        auto model_stream = std::istringstream(factory.getAsString("morphemmodel"));
        model = std::make_unique<KerasModel>(model_stream);
    }

    void split(analyze::WordFormPtr forms) const;
};

} // namespace ml
