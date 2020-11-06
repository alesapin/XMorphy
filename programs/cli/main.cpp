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
#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/ml/SingleWordDisambiguate.h>
#include <xmorphy/morph/Processor.h>
#include <xmorphy/morph/WordFormPrinter.h>
#include <xmorphy/utils/UniString.h>
#include <tabulate/table.hpp>

using namespace tabulate;


#include <boost/program_options.hpp>

using namespace X;
using namespace std;
using namespace utils;


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
bool processCommandLineOptions(int argc, char ** argv, Options & opts)
{
    try
    {
        po::options_description desc("XMorphy morphological analyzer for Russian language.");
        desc.add_options()("input,i", po::value<string>(&opts.inputFile), "set input file")(
            "output,o", po::value<string>(&opts.outputFile), "set output file")("disambiguate,d", "disambiguate single word")(
            "context-disambiguate,c", "disambiguate with context")("morphem-split,m", "split morphemes")("json,j", "json");

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
        std::cerr << "Unknown error!"
                  << "\n";
        return false;
    }
    return true;
}


Table printTabular(Table & word_form_table, const WordFormPtr & form, bool with_phem)
{
    using Row = std::vector<variant<std::string, const char *, Table>>;
    for (const auto & mi : form->getMorphInfo())
    {
        Row mi_row;
        mi_row.push_back(form->getWordForm().getRawString());
        mi_row.push_back(mi.normalForm.toLowerCase().getRawString());
        mi_row.push_back(to_string(mi.sp));
        mi_row.push_back(to_string(mi.tag));
        mi_row.push_back(to_string(mi.at));
        mi_row.push_back(std::to_string(mi.probability));
        if (with_phem)
        {
            if (!form->getPhemInfo().empty())
                mi_row.push_back(WordFormPrinter::writePhemInfo(form));
            else
                mi_row.push_back("");
        }
        word_form_table.add_row(mi_row);
    }
    return word_form_table;
}


int main(int argc, char ** argv)
{
    Options opts;

    if (!processCommandLineOptions(argc, argv, opts))
        return 1;


    std::istream * is = &cin;
    std::ostream * os = &cout;
    if (!opts.inputFile.empty())
    {
        is = new ifstream(opts.inputFile);
    }
    if (!opts.outputFile.empty())
    {
        os = new ofstream(opts.outputFile);
    }
    Tokenizer tok;

    SentenceSplitter ssplitter(*is);
    Processor analyzer;
    SingleWordDisambiguate disamb;
    Disambiguator context_disamb;
    MorphemicSplitter morphemic_splitter;

    using Row = std::vector<variant<std::string, const char *, Table>>;
    Row header = {"Form", "Normal form", "Speech Part", "Morphotags", "Source", "Probability"};

    if (opts.morphemic_split)
        header.push_back("Morphemic parse");

    while (!ssplitter.eof())
    {
        Table sentence_table;
        sentence_table.add_row(header);
        for (size_t i = 0; i < header.size(); ++i)
        {
            sentence_table[0][i].format()
                .font_align(FontAlign::center)
                .font_style({FontStyle::bold});
        }

        std::string sentence;
        ssplitter.readSentence(sentence);

        std::vector<TokenPtr> tokens = tok.analyze(UniString(sentence));
        std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
        if (opts.disambiguate)
            disamb.disambiguate(forms);
        if (opts.morphemic_split || opts.context_disambiguate)
            context_disamb.disambiguate(forms);
        if (opts.morphemic_split)
            for (auto & form : forms)
                morphemic_splitter.split(form);

        for (auto & ptr : forms)
            if (!(ptr->getType() & TokenTypeTag::SEPR))
                printTabular(sentence_table, ptr, opts.morphemic_split);

        sentence_table.column(0).format()
            .multi_byte_characters(true);

        sentence_table.column(1).format()
            .multi_byte_characters(true);

        if (opts.morphemic_split)
            sentence_table.column(header.size() - 1).format()
                .multi_byte_characters(true);

        (*os) << sentence_table << std::endl;
            //(*os) << WordFormPrinter::write(ptr) << std::endl;
        os->flush();
    }
    return 0;
}
