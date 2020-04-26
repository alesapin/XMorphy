#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <array>
#include <variant>
#include <string_view>
#include <memory>
#include <boost/dll/shared_library.hpp>

namespace ml {
    using CatBoostFeature = std::variant<float, std::string>;
    typedef void ModelCalcerHandle;

    class CatBoostAPI {
    public:
        CatBoostAPI(const std::string& libPath)
            : Lib(libPath) {
            ModelCalcerCreate = Lib.get<ModelCalcerHandle*()>("ModelCalcerCreate");
            ModelCalcerDelete = Lib.get<void(ModelCalcerHandle*)>("ModelCalcerDelete");
            LoadFullModelFromFile = Lib.get<bool(ModelCalcerHandle*, const char*)>("LoadFullModelFromFile");
            GetErrorString = Lib.get<const char*()>("GetErrorString");
            CalcModelPredictionSingle = Lib.get<bool(ModelCalcerHandle*,
                                                     const float*, size_t,
                                                     const char**, size_t,
                                                     double*, size_t)>("CalcModelPredictionSingle");
            Calcer = ModelCalcerCreate();
        }
        void loadModelFromFile(const std::string& path) {
            LoadFullModelFromFile(Calcer, path.c_str());
        }

        ~CatBoostAPI() {
            ModelCalcerDelete(Calcer);
        }

        std::string getErrorString() const {
            return GetErrorString();
        }

        bool calcModelPredictionSingle(const float* floatFeatures, size_t floatFeaturesSize,
                                       const char** catFeatures, size_t catFeaturesSize,
                                       double* result, size_t resultSize) const {
            return CalcModelPredictionSingle(Calcer, floatFeatures, floatFeaturesSize, catFeatures, catFeaturesSize, result, resultSize);
        }

    private:
        ModelCalcerHandle* (*ModelCalcerCreate)();
        void (*ModelCalcerDelete)(ModelCalcerHandle* calcer);
        bool (*LoadFullModelFromFile)(ModelCalcerHandle* calcer, const char* filename);
        const char* (*GetErrorString)();
        bool (*CalcModelPredictionSingle)(ModelCalcerHandle* calcer,
                                          const float* floatFeatures, size_t floatFeaturesSize,
                                          const char** catFeatures, size_t catFeaturesSize,
                                          double* result, size_t resultSize);

        ModelCalcerHandle* Calcer;
        boost::dll::shared_library Lib;
    };

    template <size_t INPUT_NUM_SIZE, size_t INPUT_CAT_SIZE, size_t OUTPUT_SIZE>
    class CatBoostWrapper {
    public:
        using NumFeatures = std::array<float, INPUT_NUM_SIZE>;
        using CatFeatures = std::array<std::string, INPUT_CAT_SIZE>;

        CatBoostWrapper(const std::string& libPath, const std::string& modelPath)
            : Api(libPath) {
            Api.loadModelFromFile(modelPath);
        }

        size_t predictSingle(const NumFeatures& numerical, const CatFeatures& categorical) const {
            std::unique_ptr<double[]> result = std::make_unique<double[]>(OUTPUT_SIZE);
            std::unique_ptr<const char* []> raw_data = std::make_unique<const char* []>(categorical.size());
            for (size_t i = 0; i < categorical.size(); ++i) {
                raw_data[i] = categorical[i].c_str();
            }
            bool r = Api.calcModelPredictionSingle(numerical.data(), numerical.size(), raw_data.get(), categorical.size(), result.get(), OUTPUT_SIZE);
            if (!r) {
                throw std::runtime_error(std::string("Model evaluation failed:") + Api.getErrorString());
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

    private:
        CatBoostAPI Api;
    };
} // namespace ml
