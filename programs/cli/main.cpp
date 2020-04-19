#include <IO/OpCorporaIO.h>
#include <graphem/Tokenizer.h>
#include <morph/Processor.h>
#include <disamb/SingleWordDisambiguate.h>
#include <ml/Disambiguator.h>
#include <ml/MorphemicSplitter.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <sstream>

using namespace base;
using namespace disamb;
using namespace tokenize;
using namespace analyze;
using namespace ml;
using namespace std;
using namespace utils;
std::string gulp(std::istream* in) {
    std::string ret;
    if (in == &std::cin) {
        ret = std::string((std::istreambuf_iterator<char>(*in)),
                          std::istreambuf_iterator<char>());
    } else {
        char buffer[4096];
        while (in->read(buffer, sizeof(buffer)))
            ret.append(buffer, sizeof(buffer));
        ret.append(buffer, in->gcount());
    }
    return ret;
}

struct Options
{
    std::string inputFile;
    std::string outputFile;
    bool disambiguate = false;
    bool context_disambiguate = false;
    bool morphemic_split = false;
    bool json = false;
};

namespace po = boost::program_options;
namespace fs = boost::filesystem;
bool processCommandLineOptions(int argc, char ** argv, Options & opts)
{
    try
    {
        po::options_description desc(
            "XMorphy morphological analyzer for Russian language.");
        desc.add_options()
            ("input,i", po::value<string>(&opts.inputFile), "set input file")
            ("output,o", po::value<string>(&opts.outputFile), "set output file")
            ("disambiguate,d", "disambiguate single word")
            ("context-disambiguate,c", "disambiguate with context")
            ("morphem-split,m", "split morphemes")
            ("json,j", "json");

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

        if (vm.count("json"))
            opts.json = true;

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
        std::cerr << "Unknown error!" << "\n";
        return false;
    }
    return true;
}


int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    Options opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;


    std::istream* is = &cin;
    std::ostream* os = &cout;
    if (!opts.inputFile.empty()) {
        is = new ifstream(opts.inputFile);
    }
    if (!opts.outputFile.empty()) {
        os = new ofstream(opts.outputFile);
    }
    io::OpCorporaIO opprinter;
    Tokenizer tok;

    Processor analyzer;
    SingleWordDisambiguate disamb;
    Disambiguator context_disamb;
    MorphemicSplitter splitter;

    while (is->good() || is == &std::cin) {
        std::string inpfile = gulp(is);

        std::vector<TokenPtr> tokens = tok.analyze(UniString(inpfile));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
        if (opts.disambiguate)
            disamb.disambiguate(forms);
        if (opts.context_disambiguate)
            context_disamb.disambiguate(forms);
        if (opts.morphemic_split)
        {
            for (auto & form : forms)
                splitter.split(form);
        }

        for (auto& ptr : forms) {
            (*os) << opprinter.write(ptr) << "\n";
        }
        os->flush();
    }
    return 0;
}
