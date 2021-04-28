#pragma once

#include <memory>
#include <xmorphy/ml/KerasModel.h>
#include <xmorphy/morph/WordForm.h>
#include <xmorphy/build/PhemDict.h>

namespace X
{

class MorphemicSplitter
{
private:
    std::unique_ptr<KerasModel> model;
    size_t sequence_size;

    std::unique_ptr<PhemDict> phem_dict;


public:
    MorphemicSplitter(std::istream & model_stream_, size_t sequence_size_, std::istream & phem_dict_stream_)
        : model(std::make_unique<KerasModel>(model_stream_))
        , sequence_size(sequence_size_)
        , phem_dict(PhemDict::loadFromFiles(phem_dict_stream_))
    {
    }

    MorphemicSplitter();

    void split(WordFormPtr form) const;

    std::vector<PhemTag> split(const UniString & word, UniSPTag sp, UniMorphTag tag) const;
};

}
