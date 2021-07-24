#include <chrono>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <xmorphy/ml/JoinedModel.h>
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
    JoinedModel joiner;
    //size_t average_ms = 0;
    std::vector<size_t> forms_size;
    size_t prev_tokens = 0;
    std::vector<size_t> forms_lengths;
    size_t sum_ms = 0;
    do
    {
        auto start = std::chrono::system_clock::now();
        std::string sentence;
        ssplitter.readSentence(sentence);
        if (sentence.empty())
            continue;

        //std::cerr << "SEntence:" << sentence << std::endl;
        std::vector<TokenPtr> tokens = tok.analyze(UniString(sentence));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
        //joiner.disambiguateAndMorphemicSplit(forms);
        //disamb.disambiguate(forms);
        context_disamb.disambiguate(forms);
        for (const auto & form : forms)
            morphemic_splitter.split(form);

        sum_ms += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count();

        size_t sentence_length = 0;
        for (auto & form : forms)
        {
            if (form->getTokenType() & TokenTypeTag::WORD)
            {
                sentence_length += 1;
                forms_lengths.push_back(form->getWordForm().length());
            }
            forms_size.push_back(sentence_length);
        }
        tokens_total += sentence_length;

        if (tokens_total - prev_tokens > 10000)
        {
            std::cerr << tokens_total - prev_tokens << " tokens processed in " << sum_ms / static_cast<double>(1000) << "ms" << " (avg sentence len: " << std::accumulate(forms_size.begin(), forms_size.end(), 0) / static_cast<double>(forms_size.size()) << ", avg word length: " << std::accumulate(forms_lengths.begin(), forms_lengths.end(), 0) / static_cast<double>(forms_lengths.size()) << ")" << std::endl;
            forms_size.clear();
            forms_lengths.clear();
            prev_tokens = tokens_total;
            sum_ms = 0;
        }
    } while(!ssplitter.eof());

    std::cerr << "All " << tokens_total << " tokens processed in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - global_start).count() << "ms" << std::endl;

    return 0;
}
