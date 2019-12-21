#include <graphem/Tokenizer.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <fstream>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace base;
using namespace tokenize;

namespace po = boost::program_options;
namespace fs = boost::filesystem;
static constexpr auto BATCH_SIZE = 1024 * 1024;

std::vector<std::string> processBatch(const Tokenizer& tok, const std::string& batch) {
    std::vector<std::string> result;
    std::vector<base::TokenPtr> tokens = tok.analyze(utils::UniString(batch));
    std::string currentSentence;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i]->getTag().contains(GraphemTag::EXCLAMATION_MARK) ||
            tokens[i]->getTag().contains(GraphemTag::QUESTION_MARK) ||
            tokens[i]->getTag().contains(GraphemTag::DOT) ||
            tokens[i]->getTag().contains(GraphemTag::THREE_DOTS)) {
            if (tokens[i - 1]->getType() == TokenTypeTag::WORD && tokens[i - 1]->getInner().length() == 1)
                continue;
            if (currentSentence.empty())
                continue;
            currentSentence.pop_back();
            result.push_back(currentSentence);
            currentSentence = "";
        }
        if (tokens[i]->getType() == TokenTypeTag::WORD || tokens[i]->getType() == TokenTypeTag::WRNM || tokens[i]->getType() == TokenTypeTag::NUMB)
            currentSentence += tokens[i]->getInner().getRawString() + " ";
    }
    return result;
}

int main(int argc, char** argv) {
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);
    po::options_description desc(
        "Janitor clears garbage from text's");
    desc.add_options()("input,i", po::value<std::string>(), "Input file")("output,o", po::value<std::string>(), "Output file");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::ifstream inp(vm["input"].as<std::string>());
    std::ofstream ofs(vm["output"].as<std::string>());

    Tokenizer tok;
    std::string line;
    std::string buf;
    while (std::getline(inp, line)) {
        if (line.empty())
            continue;

        if (line.back() == '-') {
            line.pop_back();
            std::string nextLine;
            std::getline(inp, nextLine);
            if (nextLine.back() == '\n')
                nextLine.back() = ' ';
            line += nextLine;
        }
        if (line.back() == '\n')
            line.back() = ' '; // cut of \n symbol

        buf += line;
        if (buf.size() > BATCH_SIZE) {
            auto batch = processBatch(tok, buf);
            for (const auto& line : batch)
                ofs << line << "\n";
            buf = "";
        }
    }
    auto batch = processBatch(tok, buf);
    for (const auto& line : batch)
        ofs << line << "\n";

    return 0;
}
