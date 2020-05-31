#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/morph/DictMorphAnalyzer.h>
#include <xmorphy/morph/WordFormPrinter.h>
#include <xmorphy/utils/UniString.h>

using namespace X;
using namespace utils;

struct WordWithInfo
{
    UniString word;
    std::vector<PhemTag> pheminfo;
};

std::optional<WordWithInfo> getNounAdjForm(const UniString & orig, const UniString & word, std::vector<PhemTag> & source_phem)
{
    if (orig.length() > word.length() || orig == word)
        return {};

    size_t common_part = 0;
    for (size_t i = 0; i < orig.length(); ++i)
    {
        if (orig[i] == word[i])
            common_part++;
        else
            break;
    }
    if (common_part != source_phem.size())
    {
        auto first_tag = source_phem[common_part];
        if (first_tag != PhemTag::END)
        {
            return {};
        }

        for (size_t i = common_part + 1; i < source_phem.size(); ++i)
        {
            if (source_phem[i] != first_tag)
            {
                return {};
            }
        }
    }

    std::vector<PhemTag> result = source_phem;
    result.insert(result.end(), word.length() - result.size(), PhemTag::END);
    return WordWithInfo{word, std::move(result)};
}


std::optional<WordWithInfo> getVerbForm(const UniString & orig, const UniString & word, std::vector<PhemTag> & source_phem)
{
    if (orig == word)
        return {};
    size_t minlen = std::min(orig.length(), word.length());
    size_t common_part = 0;
    for (size_t i = 0; i < minlen; ++i)
    {
        if (orig[i] == word[i])
            common_part++;
        else
            break;
    }
    /// Root changed
    if (source_phem[common_part] == PhemTag::ROOT)
        return {};

    std::vector<PhemTag> result(source_phem.begin(), source_phem.begin() + common_part);

    size_t left_letters = word.length() - result.size();

    if (word.endsWith(UniString("ИТЕ")) && orig.endsWith(UniString("ИТЬ")) && left_letters < 3)
    {
        while (word.length() - result.size() < 3)
            result.pop_back();
        left_letters = 3;
    }
    else if (word.endsWith(UniString("ИТЕСЬ")) && orig.endsWith(UniString("ИТЬСЯ")) && left_letters < 5)
    {
        while (word.length() - result.size() < 5)
            result.pop_back();
        left_letters = 5;
    }
    else if (word[common_part] == u'Л' || word[common_part] == u'Й' || word[common_part] == u'Я')
    {
        result.push_back(PhemTag::B_SUFF);
        left_letters = word.length() - result.size();
    }
    else if (word.endsWith(UniString("ВШИ")) || word.endsWith(UniString("ВШИСЬ")))
    {
        result.push_back(PhemTag::B_SUFF);
        result.push_back(PhemTag::SUFF);
        result.push_back(PhemTag::SUFF);
        if (result.size() < word.length())
            result.insert(result.end(), word.length() - result.size(), PhemTag::END);
        return WordWithInfo{word, result};
    }
    else if (word.endsWith(UniString("В")) && left_letters == 1)
    {
        result.push_back(PhemTag::B_SUFF);
        return WordWithInfo{word, result};
    }

    if (word.endsWith(UniString("СЯ")) || word.endsWith(UniString("СЬ")))
    {
        if (left_letters > 2)
            result.insert(result.end(), left_letters - 2, PhemTag::END);
        result.insert(result.end(), 2, PhemTag::POSTFIX);
    }
    else
    {
        result.insert(result.end(), left_letters, PhemTag::END);
    }

    return WordWithInfo{word, result};
}

std::vector<PhemTag> parseTags(const UniString & tag_line)
{
    std::vector<PhemTag> result;
    auto parts = tag_line.split('/');

    for(const auto & part : parts)
    {
        auto morph_and_tag = part.split(':');
        size_t count = morph_and_tag[0].length();
        PhemTag tag;
        from_string(morph_and_tag[1].getRawString(), tag);
        if (tag != PhemTag::ROOT && tag != PhemTag::SUFF && tag != PhemTag::PREF)
            result.insert(result.end(), count, tag);
        else
        {
            if (tag == PhemTag::ROOT)
                result.push_back(PhemTag::B_ROOT);
            else if (tag == PhemTag::SUFF)
                result.push_back(PhemTag::B_SUFF);
            else
                result.push_back(PhemTag::B_PREF);
            count--;
            if (count > 0)
                result.insert(result.end(), count, tag);
        }

    }

    return result;
}

bool validateParse(const std::vector<PhemTag> & tags)
{
    for (auto t : tags)
        if (t == PhemTag::ROOT)
            return true;
    return false;
}
void dumpLexeme(const UniString & word, const std::vector<PhemTag> & tags, const std::vector<ParsedPtr> & lexeme)
{
    std::cerr << "========LEXEME OF:" << WordFormPrinter::writePhemInfo(word, tags) << "========\n";
    for (const auto & parsed : lexeme)
    {
        std::cerr << "PARSED:" << parsed->wordform.toLowerCase() << " SP:" << parsed->sp <<  " TAG:" << parsed->mt << std::endl;
    }
}

int main (int argc, char ** argv)
{
    DictMorphAnalyzer analyzer;
    std::istream & is = std::cin;
    std::string line;

    while(!is.eof())
    {
        std::getline(is, line);
        if (line.empty())
            break;
        UniString curpair(line);
        auto word_and_parse = curpair.split('\t');
        const auto & curword = word_and_parse[0];
        const auto & parse = word_and_parse[1];
        std::vector<PhemTag> orig_info = parseTags(parse);
        auto lexeme = analyzer.generate(curword.toUpperCase());
        if (lexeme.empty())
            continue;
        auto sp = lexeme[0]->sp;
        std::cout << curword << '\t' << WordFormPrinter::writePhemInfo(curword, orig_info)  << '\t' << sp << std::endl;
        for (auto ptr : lexeme)
        {
            if (ptr->wordform == curword)
                continue;


            std::optional<WordWithInfo> result;
            if ((ptr->sp == UniSPTag::NOUN || ptr->sp == UniSPTag::ADJ) && sp != UniSPTag::VERB)
            {
                result = getNounAdjForm(curword.toUpperCase(), ptr->wordform, orig_info);
            }
            else if (ptr->sp == UniSPTag::VERB && sp != UniSPTag::NOUN)
            {
                result = getVerbForm(curword.toUpperCase(), ptr->wordform, orig_info);
            }
            if (result && validateParse(result->pheminfo))
            {
                auto word = result->word.toLowerCase();
                std::cout << word << '\t' << WordFormPrinter::writePhemInfo(word, result->pheminfo) << '\t' << ptr->sp << std::endl;
            }
        }
    }

    return 0;
}
