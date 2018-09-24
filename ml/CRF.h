#ifndef _CRF_H
#define _CRF_H
#include <crfpp.h>
#include "../tag/SpeechPartTag.h"
#include "../utils/UniString.h"
#include <stdexcept>
#include <exception>
#include <algorithm>
namespace ml {

class ToStringConvertException : public std::runtime_error {
public:
    ToStringConvertException(const std::string type)
        : std::runtime_error("Unknown type: " + type) {
    }
};

template <std::size_t INPUT_SIZE>
class CRF {
private:
    std::shared_ptr<CRFPP::Tagger> tagger;

    template <std::size_t I = 0, typename... Args>
    inline typename std::enable_if<I == sizeof...(Args), void>::type
    addToTagger(const std::tuple<Args...>& values, std::string& result) {
        result.pop_back();
        tagger->add(result.c_str());
    }
    template <std::size_t I = 0, typename... Args>
        inline typename std::enable_if < I<sizeof...(Args), void>::type
                                         addToTagger(const std::tuple<Args...>& values, std::string& result) {
        std::string tmp = toString<typename std::remove_const<typename std::remove_reference<decltype(std::get<I>(values))>::type>::type>(std::get<I>(values));
        addToTagger<I + 1, Args...>(values, result + tmp + "\t");
    }

public:
    CRF() = default;
    CRF(const std::string& modelPath) {
        tagger = std::shared_ptr<CRFPP::Tagger>(CRFPP::createTagger(("-m " + modelPath).c_str()));
        tagger->clear();
    }

    void setModel(const std::string& path) {
        tagger = std::shared_ptr<CRFPP::Tagger>(CRFPP::createTagger(("-m " + path).c_str()));
        tagger->clear();
    }

    template <typename T>
    std::string toString(const T& obj) {
        if (std::is_same<T, utils::UniString>::value) {
            return obj.toUpperCase().getRawString();
        } else if (std::is_same<T, std::size_t>::value) {
            return std::to_string(obj);
        }
        throw ToStringConvertException(typeid(obj).name());
    }

    template <typename... Args,
              typename = typename std::enable_if<sizeof...(Args) == INPUT_SIZE>::type>
    std::vector<std::string> classifyBase(const std::vector<std::tuple<Args...>>& values) const {
        tagger->clear();
        for (std::size_t i = 0; i < values.size(); ++i) {
            addToTagger<0, Args...>(values[i], "");
        }
        if (!tagger->parse())
            return std::vector<std::string>();
        std::vector<std::string> result(values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            result[i] = tagger->y2(i);
        }
        return result;
    }

    virtual std::vector<std::string> classifyBase(const std::vector<std::array<std::string, INPUT_SIZE>>& values) const {
        tagger->clear();
        for (std::size_t i = 0; i < values.size(); ++i) {
            std::string data = "";
            for (std::size_t j = 0; j < INPUT_SIZE; ++j) {
                data += values[i][j] + "\t";
            }
            data.pop_back();
            tagger->add(data.c_str());
        }
        if (!tagger->parse())
            return std::vector<std::string>();
        std::vector<std::string> result(values.size());
        for (std::size_t i = 0; i < values.size(); ++i) {
            result[i] = tagger->y2(i);
        }
        return result;
    }
    virtual ~CRF() {}
};
}
#endif
