#include <fdeep/fdeep.hpp>
#include <IO/OpCorporaIO.h>
#include <ml/Disambiguator.h>
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

void testModel(const ml::Disambiguator & disamb, Tokenizer & tok, Processor & analyzer, const std::string & sentence)
{
    std::vector<TokenPtr> tokens = tok.analyze(UniString(sentence));

    std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
    std::vector<WordFormPtr> test_forms;
    for (size_t i = 0; i < forms.size(); ++i) {
        if (!(forms[i]->getTag() & base::GraphemTag::SPACE))
            test_forms.push_back(forms[i]);
    }

    io::OpCorporaIO opprinter;
    disamb.disambiguate(test_forms);
    for (size_t i = 0; i < test_forms.size(); ++i) {
        std::cerr << opprinter.write(test_forms[i]) << std::endl;
    }
}

int main()
{
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    Tokenizer tok;
    Processor analyzer;
    std::string words =
        "привет"
        " прекрасный"
        " мир"
        " привет"
        " прекрасный"
        " дивный"
        " день";

    std::ifstream embedding_stream("/home/alesap/code/cpp/XMorpheWork/data/models/morphorueval_cbow.embedding_50.bin");
    std::ifstream model_stream("/home/alesap/code/cpp/XMorpheWork/data/models/complex_model_50.json");
    ml::Disambiguator disamb(embedding_stream, model_stream, 7);
    testModel(disamb, tok, analyzer, words);
    std::cerr << "=====\n";
    testModel(disamb, tok, analyzer, "мама наверняка мыла раму и грязный пол");
    std::cerr << "=====\n";
    return 0;
    return 0;
}
