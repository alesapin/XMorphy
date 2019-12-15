#include <build/RawDict.h>
#include <build/ParadigmBuilder.h>
#include <build/MorphDict.h>
#include <build/SuffixDict.h>
#include <build/DictBuilder.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <optional>

// Tool for build of binary dictionaries
// for XMorphy morph analyzer

using namespace base;
using namespace build;
using namespace std;
using namespace utils;

struct OptionsPaths {
    std::string xml_dict;
    std::string main_dict;
    std::string affix_dict;
    std::string suffix_dict;
};

namespace po = boost::program_options;
namespace fs = boost::filesystem;
bool processCommandLineOptions(int argc, char ** argv, OptionsPaths & opts)
{
    try
    {
        po::options_description desc(
            "Dictionaries builder for XMorphy morphological analyzer");
        desc.add_options()
            ("xml-dict,x", po::value<string>(&opts.xml_dict)->required(), "Input opencorpora dictionary in xml format")
            ("main-dict,m", po::value<string>(&opts.main_dict)->required(), "Main dictionary output file name")
            ("affix-dict,a", po::value<string>(&opts.affix_dict)->required(), "Affix dictionary output file name")
            ("suffix-dict,n", po::value<string>(&opts.suffix_dict)->required(), "Suffixes dictionary output file name");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
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

    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    OptionsPaths opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;

    std::shared_ptr<RawDict> rawDict;
    std::cerr << "Building raw dict from xml file\n";
    clock_t build_begin = clock();
    buildRawDictFromXML(rawDict, opts.xml_dict);
    clock_t build_end = clock();
    std::cerr << "Build finished in " << (double(build_end - build_begin) / CLOCKS_PER_SEC) << "\n";

    std::unique_ptr<MorphDict> morph_dict;
    OpenCorporaParadigmBuilder paradigm_builder;
    std::map<Paradigm, std::pair<std::size_t, std::size_t>> paradigms = paradigm_builder.getParadigms(rawDict);

    UniMap prefs, sufs;
    TagMap tags;
    std::tie(prefs, tags, sufs) = splitParadigms(paradigms);
    std::map<EncodedParadigm, std::size_t> epars = encodeParadigms(paradigms, prefs, tags, sufs);

    DictBuilder dict_builder(paradigms, epars, prefs, tags, sufs);
    dict_builder.buildMorphDict(morph_dict, rawDict);

    clock_t begin = clock();
    dropToFiles(morph_dict, opts.main_dict, opts.affix_dict);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    std::cerr << "Drop finished loading: " << elapsed_secs << "\n";

    std::unique_ptr<SuffixDict> suffix_dict;

    dict_builder.buildSuffixDict(suffix_dict, rawDict);

    dropToFiles(suffix_dict, opts.suffix_dict);
}
