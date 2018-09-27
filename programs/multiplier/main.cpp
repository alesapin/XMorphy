#include <graphem/Tokenizer.h>
#include <IO/OpCorporaIO.h>
#include <morph/Processor.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

using namespace base;
using namespace tokenize;
using namespace analyze;
using namespace std;
using namespace utils;

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;


int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    std::string dpath = "./dicts";
    po::options_description desc(
        "WordForm multiplier");
    desc.add_options()
        ("input,i", po::value<string>(), "Input file")
        ("output,o", po::value<string>(), "Output file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    io::OpCorporaIO opprinter;
    Tokenizer tok;
    Processor anal(dpath + "/realdict_x", dpath + "/real_affixes_x.txt",
                   dpath + "/prefix_dict", dpath + "/realsuffixdict_x",
                   dpath + "/hyph_dict.txt");

    std::ifstream inp(vm["input"].as<std::string>());
    std::ofstream ofs(vm["output"].as<std::string>());

    std::string word, parse;
    pt::ptree root;
    size_t i = 0;
    while(inp >> word >> parse) {
        TokenPtr token = tok.analyzeSingleWord(UniString(word));
        WordFormPtr wf = anal.analyzeSingleToken(token);
        std::vector<base::PhemTag> phemInfo = opprinter.readPhemInfo(parse);
        wf->setPhemInfo(phemInfo);
        auto sub = opprinter.writeToJSON(wf);
        root.add_child(std::to_string(i++) + "_" + wf->getWordForm().getRawString(), sub);
    }
    pt::write_json(ofs, root);
    return 0;
}
