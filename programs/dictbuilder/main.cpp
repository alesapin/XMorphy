#include <build/RawDict.h>
#include <build/ParadigmBuilder.h>
#include <build/MorphDict.h>
#include <build/SuffixDict.h>
#include <build/DictBuilder.h>
#include <boost/program_options.hpp>
#include <chrono>
#include <optional>

// Tool for build of binary dictionaries
// for XMorphy morph analyzer

using namespace X;
using namespace std;
using namespace utils;

struct OptionsPaths {
    std::string tsv_dict;
    std::string main_dict;
    std::string affix_dict;
    std::string suffix_dict;

    std::string corpus_path;
    std::string disamb_dict;
};

namespace po = boost::program_options;
bool processCommandLineOptions(int argc, char ** argv, OptionsPaths & opts)
{
    try
    {
        po::options_description main(
            "Main dictionaries builder for XMorphy morphological analyzer");
        main.add_options()
            ("tsv-dict,t", po::value<string>(&opts.tsv_dict), "Input opencorpora dictionary in xml format")
            ("main-dict,m", po::value<string>(&opts.main_dict), "Main dictionary output file name")
            ("affix-dict,a", po::value<string>(&opts.affix_dict), "Affix dictionary output file name")
            ("suffix-dict,n", po::value<string>(&opts.suffix_dict), "Suffixes dictionary output file name");

        po::options_description disamb(
            "Disambiguate dictionaries builder for XMorphy morphological analyzer");
        disamb.add_options()
            ("corpus-path,c", po::value<string>(&opts.corpus_path), "Input corpus for disambiguation dictionary")
            ("disamb-dict,d", po::value<string>(&opts.disamb_dict), "Disambiguation dictionary name");

        po::options_description all("Dictionaries builder");
        all.add(main).add(disamb);
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, all), vm);

        if (vm.count("help"))
        {
            std::cout << all << "\n";
            return false;
        }

        po::notify(vm);
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

    OptionsPaths opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;

    if (!opts.corpus_path.empty() && !opts.tsv_dict.empty())
        throw std::runtime_error("You have to specify corpus or tsv dict. Not together.");

    if (!opts.tsv_dict.empty()) {
        std::cerr << "Building raw dict from tsv file\n";
        clock_t build_begin = clock();
        auto rawDict = RawDict::buildRawDictFromTSV(opts.tsv_dict);
        clock_t build_end = clock();
        std::cerr << "Build finished in " << (double(build_end - build_begin) / CLOCKS_PER_SEC) << "\n";

        ParadigmBuilder paradigm_builder;
        std::map<Paradigm, ParadigmOccurences> paradigms = paradigm_builder.getParadigms(rawDict);

        IntermediateParadigmsState intermediateState = splitParadigms(paradigms);
        std::map<EncodedParadigm, std::size_t> epars = encodeParadigms(paradigms, intermediateState);

        DictBuilder dict_builder(paradigms, epars, intermediateState);
        std::unique_ptr<MorphDict> morph_dict = dict_builder.buildMorphDict(rawDict);

        clock_t begin = clock();
        dropToFiles(morph_dict, opts.main_dict, opts.affix_dict);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cerr << "Drop finished loading: " << elapsed_secs << "\n";

        std::unique_ptr<SuffixDict> suffix_dict = dict_builder.buildSuffixDict(rawDict);

        dropToFiles(suffix_dict, opts.suffix_dict);
    } else if (!opts.corpus_path.empty()) {
        std::ifstream ifs(opts.corpus_path);
        clock_t build_begin = clock();
        std::unique_ptr<DisambDict> dict = buildDisambDict(ifs);
        clock_t build_end = clock();
        std::cerr << "Build finished in " << (double(build_end - build_begin) / CLOCKS_PER_SEC) << "\n";
        clock_t begin = clock();
        dropToFiles(dict, opts.disamb_dict);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cerr << "Drop finished loading: " << elapsed_secs << "\n";
    } else {
        throw std::runtime_error("Corpus path or tsv dict have to be specified.");
    }
    return 0;
}
