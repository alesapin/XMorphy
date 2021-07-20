#include <chrono>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <xmorphy/graphem/Tokenizer.h>
#include <xmorphy/graphem/SentenceSplitter.h>
#include <xmorphy/ml/Disambiguator.h>
#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/ml/SingleWordDisambiguate.h>
#include <xmorphy/morph/Processor.h>
#include <xmorphy/morph/WordFormPrinter.h>
#include <xmorphy/utils/UniString.h>
#include <xmorphy/morph/PrettyFormater.h>
#include <xmorphy/morph/TSVFormater.h>
#include <xmorphy/morph/JSONEachSentenceFormater.h>
#include <boost/program_options.hpp>

using namespace X;
using namespace std;

int main(int argc, char *argv[]) {
    Tokenizer tok;
    SentenceSplitter ssplitter(std::cin);
    Processor analyzer;
    //SingleWordDisambiguate disamb;
    Disambiguator context_disamb;
    MorphemicSplitter morphemic_splitter;
    size_t tokens_total = 0;
    auto global_start = std::chrono::system_clock::now();
    auto start = std::chrono::system_clock::now();
    //size_t average_ms = 0;
    do
    {
        std::string sentence;
        ssplitter.readSentence(sentence);
        if (sentence.empty())
            continue;

        std::vector<TokenPtr> tokens = tok.analyze(UniString(sentence));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
        //disamb.disambiguate(forms);
        //context_disamb.disambiguate(forms);
        //for (const auto & form : forms)
        //    morphemic_splitter.split(form);

        tokens_total += forms.size();
        if (tokens_total % 1000 == 0)
        {
            size_t current_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
            std::cerr << "1000 tokens processed in " << current_ms << "ms" << std::endl;
            start = std::chrono::system_clock::now();
        }
    } while(!ssplitter.eof());

    std::cerr << "All " << tokens_total << " tokens processed in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - global_start).count() << "ms" << std::endl;

    return 0;
}
