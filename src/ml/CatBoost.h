#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include <variant>
#include <string_view>
#include <memory>
#include <catboost/libs/model_interface/model_calcer_wrapper.h>

namespace ml {
    using CatBoostFeature = std::variant<float, std::string>;
    template <size_t INPUT_NUM_SIZE, size_t INPUT_CAT_SIZE, size_t OUTPUT_SIZE>
    class CatBoostWrapper {
    public:
        CatBoostWrapper(const std::string& modelPath)
            : Calcer(ModelCalcerCreate()) {
            LoadFullModelFromFile(Calcer, modelPath.data());
        }

        size_t predictSingle(const std::array<float, INPUT_NUM_SIZE>& numerical, const std::array<std::string, INPUT_CAT_SIZE>& categorical) {
            std::unique_ptr<double[]> result = std::make_unique<double[]>(OUTPUT_SIZE);
            bool r = CalcModelPredictionSingle(Calcer, numerical.data(), numerical.size(), categorical.data(), categorical.size(), result.get(), OUTPUT_SIZE);
            if (!r) {
                throw std::runtime_error(std::string("Model evaluation failed:") + GetErrorString());
            }
            auto maxElem = std::max_element(result.get(), result.get() + OUTPUT_SIZE);
            return maxElem - result.get();
        }

        virtual ~CatBoostWrapper() {
            ModelCalcerDelete(Calcer);
        }

    private:
        ModelCalcerHandle* Calcer;
    };
} // namespace ml
