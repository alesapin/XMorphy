#include <build/RawDict.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <morph/WordForm.h>
#include <memory>
#include <set>
#include <tag/OpCorporaUDConverter.h>
#include <iostream>
#include <unordered_set>

using namespace base;
using namespace build;
using namespace std;
using namespace utils;
using namespace analyze;


struct OptionsPaths {
    std::string xml_dict;
    std::string text_dict;
    std::string conf_path;
};

void dumpWordForm(std::ostream & os, WordFormPtr wf)
{
    const auto& morphInfo = *wf->getMorphInfo().begin();
    os << wf->getWordForm() << '\t';
    os << morphInfo.normalForm << '\t';
    os << morphInfo.usp << '\t';
    os << morphInfo.utag << std::endl;
}

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
            ("text-dict,t", po::value<string>(&opts.text_dict)->required(), "Text dictionary output file name")
            ("conf-path,c", po::value<string>(&opts.conf_path)->required(), "Path to json config for conversion");

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

    OpCorporaUDConverter converter(opts.conf_path);
    std::cerr << "Building raw dict from xml file\n";
    clock_t build_begin = clock();
    auto rawDict = RawDict::buildRawDictFromXML(opts.xml_dict);
    clock_t build_end = clock();
    std::cerr << "Build finished in " << (double(build_end - build_begin) / CLOCKS_PER_SEC) << "\n";

    std::ofstream ofs(opts.text_dict);
    for(size_t i = 0; i < rawDict.size(); ++i)
    {
        if (i % 1000 == 0)
            std::cerr << "Processed i:" << i << std::endl;
        const auto & [words, tags] = rawDict[i];
        const UniString & normalForm = words.front();
        std::cerr << "NF:" << normalForm << std::endl;
        std::unordered_set<std::string> duplicateFilter;
        for (size_t word = 0; word < words.size(); ++word)
        {
            analyze::MorphInfo info(
                normalForm,
                std::get<0>(tags[word]),
                std::get<1>(tags[word]),
                0.0,
                base::AnalyzerTag::UNKN,
                0
            );

            WordFormPtr wf = std::make_shared<WordForm>(words[word], std::set{info});
            converter.convert(wf);
            if (wf->getMorphInfo().size() == 0) {
                std::cerr << "Bad WF:" << wf->getWordForm() << std::endl;
                continue;
            }
            std::ostringstream oss;
            dumpWordForm(oss, wf);
            auto str = oss.str();
            if (!duplicateFilter.count(str))
            {
                ofs << str;
                duplicateFilter.insert(str);
            }
        }
        ofs << std::endl;
    }

    return 0;
}
