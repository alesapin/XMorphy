#include <iostream>
#include <memory>
#include <set>
#include <unordered_set>
#include <boost/program_options.hpp>
#include <xmorphy/morph/WordForm.h>
#include "OpCorporaUDConverter.h"
#include "XMLDictLoader.h"

using namespace X;
using namespace std;


struct OptionsPaths
{
    std::string xml_dict;
    std::string text_dict;
    std::string conf_path;
};

void dumpWordForm(std::ostream & os, const ConvertWordForm & wf)
{
    for (const auto & morphInfo : wf.infos)
    {
        os << wf.wordForm.replace(u'Ё', u'Е') << '\t';
        os << morphInfo.normalForm.replace(u'Ё', u'Е') << '\t';
        os << morphInfo.usp << '\t';
        os << morphInfo.utag << '\t';
        os << wf.isNormalForm << std::endl;
    }
}

namespace po = boost::program_options;
bool processCommandLineOptions(int argc, char ** argv, OptionsPaths & opts)
{
    try
    {
        po::options_description desc("Dictionaries builder for XMorphy morphological analyzer");
        desc.add_options()("xml-dict,x", po::value<string>(&opts.xml_dict)->required(), "Input opencorpora dictionary in xml format")(
            "text-dict,t", po::value<string>(&opts.text_dict)->required(), "Text dictionary output file name")(
            "conf-path,c", po::value<string>(&opts.conf_path)->required(), "Path to json config for conversion");

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
        std::cerr << "Unknown error!"
                  << "\n";
        return false;
    }
    return true;
}

int main(int argc, char ** argv)
{
    OptionsPaths opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;

    OpCorporaUDConverter converter(opts.conf_path);
    std::cerr << "Building raw dict from xml file\n";
    clock_t build_begin = clock();
    auto rawDict = buildRawDictFromXML(opts.xml_dict);
    clock_t build_end = clock();
    std::cerr << "Build finished in " << (double(build_end - build_begin) / CLOCKS_PER_SEC) << "\n";

    std::ofstream ofs(opts.text_dict);
    for (size_t i = 0; i < rawDict.size(); ++i)
    {
        if (i % 1000 == 0)
            std::cerr << "Processed i:" << i << std::endl;
        const auto & [words, tags, nf_mask] = rawDict[i];
        UniString normalForm = words.front();
        std::unordered_set<std::string> duplicateFilter;
        for (size_t word_idx = 0; word_idx < words.size(); ++word_idx)
        {
            if (nf_mask[word_idx])
            {
                normalForm = words[word_idx];
            }

            ConvertMorphInfo info{
                normalForm,
                std::get<1>(tags[word_idx]),
                std::get<0>(tags[word_idx]),
                UniMorphTag::UNKN,
                UniSPTag::X,
            };
            ConvertWordForm wf{words[word_idx], std::set{info}};
            converter.convert(wf);
            wf.isNormalForm = nf_mask[word_idx];

            if (wf.infos.size() == 0)
            {
                std::cerr << "Bad WF:" << wf.wordForm << std::endl;
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
