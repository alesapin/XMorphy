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

    io::OpCorporaIO opprinter;
    std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
    std::vector<WordFormPtr> test_forms;
    for (size_t i = 0; i < forms.size(); ++i) {
        if (!(forms[i]->getTag() & base::GraphemTag::SPACE))
        {
            test_forms.push_back(forms[i]);
        }
    }

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

    std::ifstream embedding_stream("/home/alesap/code/cpp/XMorpheWork/data/models/morphorueval_cbow.embedding_50.bin");
    std::ifstream model_stream("/home/alesap/code/cpp/XMorpheWork/data/models/disamb_model_50.json");
    ml::Disambiguator disamb(embedding_stream, model_stream, 9);
    testModel(disamb, tok, analyzer, "Ехал грека через реку видит грека в реке рак");
    std::cerr << "=====\n";
    testModel(disamb, tok, analyzer, "мама наверняка мыла грязную раму");
    std::cerr << "=====\n";
    testModel(disamb, tok, analyzer, "Несмотря на это процесс установки софта может в той или иной степени отличаться в зависимости от дистрибутива");
    std::cerr << "=====\n";
    testModel(disamb, tok, analyzer, "Сяпала Калуша с Калушатами по напушке. И увазила Бутявку, и волит.");
    std::cerr << "=====\n";
    return 0;
}
