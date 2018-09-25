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
        using NumFeatures = std::array<float, INPUT_NUM_SIZE>;
        using CatFeatures = std::array<std::string, INPUT_CAT_SIZE>;

        CatBoostWrapper(const std::string& modelPath)
            : Calcer(ModelCalcerCreate()) {
            LoadFullModelFromFile(Calcer, modelPath.data());
        }

        size_t predictSingle(const NumFeatures& numerical, const CatFeatures& categorical) const {
            std::unique_ptr<double[]> result = std::make_unique<double[]>(OUTPUT_SIZE);
            std::unique_ptr<const char *[]> raw_data = std::make_unique<const char *[]>(categorical.size());
            for (size_t i = 0; i < categorical.size(); ++i) {
                raw_data[i] = categorical[i].c_str();
            }
            bool r = CalcModelPredictionSingle(Calcer, numerical.data(), numerical.size(), raw_data.get(), categorical.size(), result.get(), OUTPUT_SIZE);
            if (!r) {
                throw std::runtime_error(std::string("Model evaluation failed:") + GetErrorString());
            }
            auto maxElem = std::max_element(result.get(), result.get() + OUTPUT_SIZE);
            return maxElem - result.get();
        }
        std::vector<size_t> predictSequence(const std::vector<NumFeatures>& numerical, const std::vector<CatFeatures>& categorical) const {
            std::vector<size_t> result;
            if (numerical.size() != categorical.size())
                throw std::runtime_error("Not equal numerical and categorical features");
            for (size_t i = 0; i < numerical.size(); ++i)
                result.push_back(predictSingle(numerical[i], categorical[i]));
            return result;
        }

        virtual ~CatBoostWrapper() {
            ModelCalcerDelete(Calcer);
        }

    private:
        ModelCalcerHandle* Calcer;
    };
} // namespace ml
