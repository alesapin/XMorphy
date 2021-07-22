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
#include <xmorphy/ml/JoinedModel.h>
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


struct Options
{
    std::string input_file;
    std::string output_file;
    bool disambiguate = false;
    bool context_disambiguate = false;
    bool morphemic_split = false;
    std::string format;
};

namespace po = boost::program_options;
bool processCommandLineOptions(int argc, char ** argv, Options & opts)
{
    try
    {
        po::options_description desc("XMorphy morphological analyzer for Russian language.");
        desc.add_options()
            ("input,i", po::value<string>(&opts.input_file), "set input file")
            ("output,o", po::value<string>(&opts.output_file), "set output file")
            ("disambiguate,d", "disambiguate single word")
            ("context-disambiguate,c", "disambiguate with context")
            ("morphem-split,m", "split morphemes")
            ("format,f", po::value<string>(&opts.format), "format to use");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return false;
        }

        po::notify(vm);
        if (vm.count("disambiguate"))
            opts.disambiguate = true;

        if (vm.count("context-disambiguate"))
            opts.context_disambiguate = true;

        if (vm.count("morphem-split"))
            opts.morphemic_split = true;
    }
    catch (const std::exception & ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
        return false;
    }
    catch (...)
    {
        std::cerr << "Unknown error!"
                  << "\n";
        return false;
    }
    return true;
}

int main(int argc, char ** argv)
{
    Options opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;

    std::istream * is = &cin;
    std::ostream * os = &cout;
    if (!opts.input_file.empty())
    {
        is = new ifstream(opts.input_file);
    }
    if (!opts.output_file.empty())
    {
        os = new ofstream(opts.output_file);
    }
    Tokenizer tok;

    SentenceSplitter ssplitter(*is);
    Processor analyzer;
    SingleWordDisambiguate disamb;
    Disambiguator context_disamb;
    JoinedModel joiner;
    MorphemicSplitter morphemic_splitter;
    FormaterPtr formater;
    if (opts.format == "TSV")
        formater = std::make_unique<TSVFormater>(opts.morphemic_split);
    else if (opts.format == "JSONEachSentence")
        formater = std::make_unique<JSONEachSentenceFormater>(opts.morphemic_split);
    else
        formater = std::make_unique<PrettyFormater>(opts.morphemic_split);
    do
    {
        std::string sentence;
        ssplitter.readSentence(sentence);
        if (sentence.empty())
            continue;

        std::vector<TokenPtr> tokens = tok.analyze(UniString(sentence));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);

        if (opts.disambiguate)
            disamb.disambiguate(forms);

        if (opts.morphemic_split && opts.context_disambiguate)
        {
            joiner.disambiguateAndMorphemicSplit(forms);
        }
        else if (opts.morphemic_split || opts.context_disambiguate)
        {
            context_disamb.disambiguate(forms);
        }

        if (opts.morphemic_split && !opts.context_disambiguate)
            for (auto & form : forms)
                morphemic_splitter.split(form);

        (*os) << formater->format(forms) << std::endl;

        os->flush();
    } while(!ssplitter.eof());
    return 0;
}
