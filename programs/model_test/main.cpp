#include <fdeep/fdeep.hpp>
#include <fasttext.h>
#include <iostream>
#include <vector>
#include <string>
#include <morph/Processor.h>
#include <graphem/Tokenizer.h>
#include <utils/UniString.h>
using namespace tokenize;
using namespace analyze;
using namespace std;
using namespace utils;
using namespace base;

int main()
{
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    Tokenizer tok;
    Processor analyzer;
    std::vector<std::string> words = {
        "привет",
        "прекрасный",
        "мир",
        "привет",
        "прекрасный",
        "дивный",
        "новый",
        "интерсный",
        "настоящий",
        "текущий",
        "мир",
        "мама",
        "мыла",
        "раму",
        "раму",
        "мыла",
        "мама",
        "сегодня",
        "завтра",
        "вчера",
    };
    fasttext::FastText fstxt;
    fstxt.loadModel("/home/alesap/code/cpp/XMorpheWork/scripts/morphorueval-2019.model.bin");
    std::cerr << "FS:" << fstxt.getDimension() << std::endl;
    std::vector<float> data;
    size_t counter = 0;
    for (const auto word : words)
    {
        std::cerr << "counter:" << counter << " word:" << word << std::endl;
        ++counter;
        fasttext::Vector v(fstxt.getDimension());
        fstxt.getWordVector(v, word);
        //std::cerr << word << " " << v << std::endl;
        std::vector<float> word_data(v.data(), v.data() + v.size());
        word_data.resize(120);
        std::vector<TokenPtr> tokens = tok.analyze(UniString(word));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);

        for (const auto & info : forms[0]->getMorphInfo()) {
            size_t index = UniSPTag::get(info.sp);
            word_data[100 + index] = 1;
        }
        data.insert(data.end(), word_data.begin(), word_data.end());
        std::cerr <<  "Data size:" << data.size() << std::endl;
    }

    const auto model = fdeep::load_model("/home/alesap/code/cpp/XMorpheWork/data/models/sp_model.json");
    std::cerr << "Dummy\n";
    std::cerr << "Inputshape:" << model.generate_dummy_inputs().size() << std::endl;
    std::cerr << "Inside input: " << fdeep::show_tensor_shape(model.generate_dummy_inputs()[0].shape()) << std::endl;
    auto result = model.predict({fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(20), static_cast<size_t>(120)), std::move(data))});
    std::cerr << "Result\n";
    std::cerr <<  fdeep::show_tensors(result) << std::endl;
    std::cerr <<  fdeep::show_tensor_shape(result[0].shape())<< std::endl;
    auto vector_begin = result[0].as_vector()->begin();
    auto vector_end = result[0].as_vector()->end();
    size_t i = 0;
    for (auto it = vector_begin; it != vector_end; it += 21)
    {
        auto max_pos = std::max_element(it, it + 21);
        auto max_index = std::distance(vector_begin, max_pos) - 21 * i;
        std::cerr << "MaxIndex for word:" << words[i] << " is " << max_index <<std::endl;
        std::cerr << "SpeechPart:" << UniSPTag::get(max_index) << std::endl;
        ++i;
    }
    return 0;
}
