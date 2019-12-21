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

std::vector<WordFormPtr> multiplyByCase(std::vector<WordFormPtr> source, const Processor& proc, const std::vector<base::PhemTag>& phemInfo) {
    std::vector<WordFormPtr> result;
    for (auto wf : source) {
        if (wf->getMorphInfo().begin()->tag.contains(base::MorphTag::Supr)) {
            result.push_back(wf);
            continue;
        }
        size_t caseSize = base::MorphTag::caseSize();
        for (size_t i = 0; i < caseSize; ++i) {
            base::MorphTag currentCase = base::MorphTag::getCase(i);
            auto tag = wf->getMorphInfo().begin()->tag;
            tag.resetIfContains(tag.getCase());
            tag |= currentCase;
            auto syntRes = proc.synthesize(wf->getWordForm(), tag);
            for (auto variant : syntRes) {
                if (variant->getMorphInfo().begin()->tag.contains(base::MorphTag::Supr))
                    continue;
                auto currentInfo = phemInfo;
                while (currentInfo.size() < variant->getWordForm().length())
                    currentInfo.push_back(base::PhemTag::ENDING);
                while (currentInfo.size() > variant->getWordForm().length())
                    currentInfo.pop_back();
                variant->setPhemInfo(currentInfo);
                result.push_back(variant);
            }
        }
    }
    return result;
}

std::vector<WordFormPtr> multiplyByGender(std::vector<WordFormPtr> source, const Processor& proc, const std::vector<base::PhemTag>& phemInfo) {
    std::vector<WordFormPtr> result;
    for (auto wf : source) {
        size_t genderSize = base::MorphTag::genderSize();
        for (size_t i = 0; i < genderSize; ++i) {
            base::MorphTag currentGender = base::MorphTag::getGen(i);
            auto tag = wf->getMorphInfo().begin()->tag;
            tag.resetIfContains(tag.getGender());
            tag |= currentGender;
            auto syntRes = proc.synthesize(wf->getWordForm(), tag);
            for (auto variant : syntRes) {
                auto currentInfo = phemInfo;
                while (currentInfo.size() < variant->getWordForm().length())
                    currentInfo.push_back(base::PhemTag::ENDING);
                while (currentInfo.size() > variant->getWordForm().length())
                    currentInfo.pop_back();
                variant->setPhemInfo(currentInfo);
                result.push_back(variant);
            }
        }
    }
    return result;
}

std::vector<WordFormPtr> multiplyByNumber(std::vector<WordFormPtr> source, const Processor& proc, const std::vector<base::PhemTag>& phemInfo) {
    std::vector<WordFormPtr> result;
    for (auto wf : source) {
        size_t numberSize = base::MorphTag::numberSize();
        for (size_t i = 0; i < numberSize; ++i) {
            base::MorphTag currentNumber = base::MorphTag::getNum(i);
            auto tag = wf->getMorphInfo().begin()->tag;
            tag.resetIfContains(tag.getNumber());
            tag |= currentNumber;
            auto syntRes = proc.synthesize(wf->getWordForm(), tag);
            for (auto variant : syntRes) {
                auto currentInfo = phemInfo;
                while (currentInfo.size() < variant->getWordForm().length())
                    currentInfo.push_back(base::PhemTag::ENDING);
                while (currentInfo.size() > variant->getWordForm().length())
                    currentInfo.pop_back();
                variant->setPhemInfo(currentInfo);
                result.push_back(variant);
            }
        }
    }
    return result;
}

int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    std::string dpath = "./dicts";
    po::options_description desc(
        "WordForm multiplier");
    desc.add_options()("input,i", po::value<string>(), "Input file")("output,o", po::value<string>(), "Output file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    io::OpCorporaIO opprinter;
    Tokenizer tok;
    std::ifstream mainIs(dpath + "/maindict.bin");
    std::ifstream affixIs(dpath + "/affixdict.bin");
    std::ifstream prefixDict(dpath + "/prefixdict.txt");
    std::ifstream suffixDict(dpath + "/suffixdict.bin");
    std::ifstream hyphDict(dpath  + "/hyphdict.txt");

    Processor anal(mainIs, affixIs, prefixDict, suffixDict, hyphDict);
    std::ifstream inp(vm["input"].as<std::string>());
    std::ofstream ofs(vm["output"].as<std::string>());

    std::string word, parse;
    pt::ptree root;
    size_t i = 0;
    while (inp >> word >> parse) {
        TokenPtr token = tok.analyzeSingleWord(UniString(word));
        WordFormPtr wf = anal.analyzeSingleToken(token);
        std::vector<base::PhemTag> phemInfo = opprinter.readPhemInfo(parse);
        wf->setPhemInfo(phemInfo);
        std::vector<WordFormPtr> wfs{wf};
        if (wf->getMorphInfo().begin()->sp == base::SpeechPartTag::NOUN || wf->getMorphInfo().begin()->sp == base::SpeechPartTag::ADJF) {
            wfs = multiplyByCase(wfs, anal, phemInfo);
        }
        if (wf->getMorphInfo().begin()->sp == base::SpeechPartTag::ADJF || wf->getMorphInfo().begin()->sp == base::SpeechPartTag::VERB) {
            wfs = multiplyByGender(wfs, anal, phemInfo);
        }
        if (wf->getMorphInfo().begin()->sp == base::SpeechPartTag::ADJF || wf->getMorphInfo().begin()->sp == base::SpeechPartTag::VERB || wf->getMorphInfo().begin()->sp == base::SpeechPartTag::NOUN) {
            wfs = multiplyByNumber(wfs, anal, phemInfo);
        }

        for (auto wform : wfs) {
            auto sub = opprinter.writeToJSON(wform);
            root.add_child(std::to_string(i++) + "_" + wform->getWordForm().getRawString(), sub);
        }
    }
    pt::write_json(ofs, root);
    return 0;
}
