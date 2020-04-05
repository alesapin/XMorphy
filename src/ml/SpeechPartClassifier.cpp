#include <ml/SpeechPartClassifier.h>

namespace ml
{

std::vector<float> SpeechPartClassifier::getVectorWithPossibleParts(const analyze::WordFormPtr & wf) const
{
    std::vector<float> result(base::UniSPTag::size());
    for (const auto& info : wf->getMorphInfo()) {
        size_t index = base::UniSPTag::get(info.sp);
        result[index] = 1;
    }
    return result;
}

std::vector<base::UniSPTag> SpeechPartClassifier::classifySpeechPart(std::vector<float> words) const {
    std::vector<base::UniSPTag> sp_vector;
    std::vector<float> result = model.predictSingle(std::move(words));
    size_t i = 0;
    size_t step = base::UniSPTag::size();
    for (auto it = result.begin(); it != result.end(); it += step) {
        auto max_pos = std::max_element(it, it + step);
        auto max_index = std::distance(result.begin(), max_pos) - (step) * i;
        sp_vector.emplace_back(base::UniSPTag::get(max_index));
        ++i;
    }

    return sp_vector;
}

} // namespace ml
