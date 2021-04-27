#include <boost/range/as_literal.hpp>
#include <xmorphy/ml/MorphemicSplitter.h>
#include <xmorphy/morph/SuffixDictAnalyzer.h>
#include <xmorphy/morph/WordFormPrinter.h>
#include <xmorphy/utils/UniString.h>

using namespace X;

struct WordWithInfo
{
    UniString word;
    std::vector<PhemTag> pheminfo;
};

std::optional<WordWithInfo> getNounAdjForm(const UniString & orig, const UniString & word, std::vector<PhemTag> & source_phem, bool is_adv, const UniSPTag & speech_part, const UniMorphTag & morph_tag)
{
    if (orig == word)
        return {};

    size_t common_part = 0;
    for (size_t i = 0; i < std::min(orig.length(), word.length()); ++i)
    {
        if (orig[i] == word[i])
            common_part++;
        else
            break;
    }

    if (common_part == 0)
    {
        std::optional<WordWithInfo> result;
        if (word.startsWith(UniString("НАИ")))
        {
            result = getNounAdjForm(orig, word.subString(3), source_phem, is_adv, speech_part, morph_tag);
            if (result)
                result->pheminfo.insert(result->pheminfo.begin(), 3, PhemTag::PREF);
        }
        else if (word.startsWith(UniString("ПРЕНАИ")))
        {
            result = getNounAdjForm(orig, word.subString(6), source_phem, is_adv, speech_part, morph_tag);
            if (result)
            {
                result->pheminfo.insert(result->pheminfo.begin(), 2, PhemTag::PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 1, PhemTag::B_PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 2, PhemTag::PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 1, PhemTag::B_PREF);
            }
        }
        else if (word.startsWith(UniString("АРХИНАИ")))
        {
            result = getNounAdjForm(orig, word.subString(7), source_phem, is_adv, speech_part, morph_tag);
            if (result)
            {
                result->pheminfo.insert(result->pheminfo.begin(), 2, PhemTag::PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 1, PhemTag::B_PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 3, PhemTag::PREF);
                result->pheminfo.insert(result->pheminfo.begin(), 1, PhemTag::B_PREF);
            }
        }
        return result;
    }
    if ((common_part == word.length() || common_part == word.length() - 1) && word.endsWith(UniString("О")))
    {
        if (orig[common_part - 1] == u'О')
            common_part--;

        auto result = std::vector<PhemTag>(source_phem.begin(), source_phem.begin() + common_part);
        result.push_back(PhemTag::SUFF);
        if (result.size() != word.length())
            return {};
        return WordWithInfo{word, result};
    }

    if (common_part == word.length())
        return WordWithInfo{word, std::vector<PhemTag>(source_phem.begin(), source_phem.begin() + word.length())};

    //if (orig.length() > word.length())
    //    return {};

    if (common_part != source_phem.size())
    {
        auto first_tag = source_phem[common_part];
        if (first_tag != PhemTag::END)
        {
            if (morph_tag & UniMorphTag::Part)
                std::cerr << word << '\t' << orig << '\t' << WordFormPrinter::writePhemInfo(orig, source_phem) << '\t' << "PART" << std::endl;
            else
                std::cerr << word << '\t' << orig << '\t' << WordFormPrinter::writePhemInfo(orig, source_phem) << '\t' << speech_part  << std::endl;
            return {};
        }

        for (size_t i = common_part + 1; i < source_phem.size(); ++i)
        {
            if (source_phem[i] != first_tag)
            {
                if (morph_tag & UniMorphTag::Part)
                    std::cerr << word << '\t' << orig << '\t' << WordFormPrinter::writePhemInfo(orig, source_phem) << '\t' << "PART" << std::endl;
                else
                    std::cerr << word << '\t' << orig << '\t' << WordFormPrinter::writePhemInfo(orig, source_phem) << '\t' << speech_part  << std::endl;

                return {};
            }
        }
    }

    if (word.length() >= orig.length())
    {
        std::vector<PhemTag> result = source_phem;
        result.insert(result.end(), word.length() - result.size(), PhemTag::END);
        return WordWithInfo{word, std::move(result)};
    }
    else
    {
        std::vector<PhemTag> result = source_phem;
        result.resize(word.length());
        return WordWithInfo{word, std::move(result)};
    }
}

bool checkNext(const UniString & word, size_t pos, const UniString & expected)
{
    if (pos + expected.length() > word.length())
        return false;
    size_t expected_pos = 0;
    for (size_t i = pos; i < word.length() && expected_pos < expected.length(); ++i, ++expected_pos)
        if (word[i] != expected[expected_pos])
            return false;
    return true;
}

std::optional<WordWithInfo> getVerbFormV2(const UniString & orig, const UniString & word, std::vector<PhemTag> & source_phem)
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

    /// Root changed and seems like we don't no how to process it
    if (source_phem[common_part] == PhemTag::ROOT)
        return {};

    if (common_part == word.length())
    {
        /// суну сунуть, крикну кринуть
        if (word.endsWith(UniString("НУ")) && orig.endsWith(UniString("НУТЬ")))
            common_part--;
        if (word == UniString("БУДУ"))
            common_part--;
    }

    if (orig.endsWith(UniString("ЫЙ")) && common_part != orig.length() && source_phem[common_part] == PhemTag::END)
    {
        std::vector<PhemTag> result = source_phem;
        if (word.length() < orig.length())
            result.resize(word.length());
        else
            result.insert(result.end(), word.length() - result.size(), PhemTag::END);
        return WordWithInfo{word, std::move(result)};
    }

    ///кипячусь кипятится
    if (word.endsWith(UniString("ЧУСЬ")) && orig.endsWith(UniString("ТИТЬСЯ")))
        common_part--;

    /// вскипячу вскипятить
    if (word.endsWith(UniString("ЧУ")) && orig.endsWith(UniString("ТИТЬ")))
        common_part--;

    if (word.endsWith(UniString("ЯТ")) && orig.endsWith(UniString("ЯТЬ"))) // выстоят выстоять
        common_part = word.length() - 2;
    else if (word.endsWith(UniString("ЯТСЯ")) && orig.endsWith(UniString("ЯТЬСЯ")))
        common_part = word.length() - 4;

    if ((word.endsWith(UniString("ИМСЯ"))) && orig.endsWith(UniString("ИТЬСЯ"))) /// учимся учиться
        common_part--;
    else if (word.endsWith(UniString("ИМ")) && orig.endsWith(UniString("ИТЬ")))
        common_part--;

    /// утопимтесь утопиться
    if (word.endsWith(UniString("ИМТЕСЬ")) && orig.endsWith(UniString("ИТЬСЯ")))
        common_part--;

    /// утопимте утопитесь
    if (word.endsWith(UniString("ИМТE")) && orig.endsWith(UniString("ИТЬСЯ")))
        common_part--;

    if (word.endsWith(UniString("ИМТE")) && orig.endsWith(UniString("ИТЬ")))
        common_part--;

    /// раскрошитесь раскрошиться
    if (word.endsWith(UniString("ТЕСЬ")) && orig.endsWith(UniString("ТЬСЯ")) && word.length() - common_part == 3)
        common_part--;

    /// раскрошите раскрошиться
    if (word.endsWith(UniString("ТЕ")) && orig.endsWith(UniString("ТЬСЯ")) && word.length() - common_part == 1)
        common_part--;



    std::vector<PhemTag> result(source_phem.begin(), source_phem.begin() + common_part);

    if (checkNext(word, common_part, UniString("ЁВ")) || (checkNext(word, common_part, UniString("ЕВ")) && !checkNext(word, common_part, UniString("ЕВА")))
        || checkNext(word, common_part, UniString("ВА"))
        || checkNext(word, common_part, UniString("ЕН"))
        || checkNext(word, common_part, UniString("ЕН"))
        || checkNext(word, common_part, UniString("ЯЧ"))
        )
    {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
        common_part += 2;
    }

    if (result.size() == word.length())
        return WordWithInfo{word, result};

    if (checkNext(word, common_part, UniString("ОВА"))
        || checkNext(word, common_part, UniString("ЕВА"))
        || checkNext(word, common_part, UniString("ЫВА"))
        || checkNext(word, common_part, UniString("ИВА"))
        || checkNext(word, common_part, UniString("ВШИ")))
    {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 2, PhemTag::SUFF);
        common_part += 3;
    }

    if (result.size() == word.length())
        return WordWithInfo{word, result};

    if (common_part == word.length() - 1 && checkNext(word, common_part, UniString("В")))
    {
        result.push_back(PhemTag::B_SUFF);
        common_part += 1;
    }

    if (result.size() == word.length())
        return WordWithInfo{word, result};

    else if (word[common_part] == u'Л'
        || word[common_part] == u'Й'
        || (word[common_part] == u'Я' &&
            (!checkNext(word, common_part, UniString("ЯТ")) || checkNext(word, common_part, UniString("ЯТЬ"))))
        || (word[common_part] == u'У' && common_part + 1 != word.length()))
    {
        result.push_back(PhemTag::B_SUFF);
        common_part += 1;
    }

    if (result.size() == word.length())
        return WordWithInfo{word, result};

    if (checkNext(word, common_part, UniString("ИТЕ"))
        || checkNext(word, common_part, UniString("ИТЬ")))
     {
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 2, PhemTag::SUFF);
        common_part += 3;
    }
    else if (checkNext(word, common_part, UniString("ТЬ"))
        || checkNext(word, common_part, UniString("ТЕ")))
    {

        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
        common_part += 2;
    }

    if (result.size() == word.length())
        return WordWithInfo{word, result};

    size_t left_letters = word.length() - common_part;

    if (word.endsWith(UniString("СЯ")) || word.endsWith(UniString("СЬ")))
    {
        if (left_letters > 2)
        {
            if (left_letters >= 4 && checkNext(word, word.length() - 4, UniString("ТЕ")))
            {
                result.insert(result.end(), left_letters - 4, PhemTag::END);
                result.push_back(PhemTag::B_SUFF);
                result.push_back(PhemTag::SUFF);
            }
            else
                result.insert(result.end(), left_letters - 2, PhemTag::END);
        }

        result.insert(result.end(), 2, PhemTag::POSTFIX);
    }
    else if ((word.endsWith(UniString("ТЕ")) && !word.endsWith(UniString("ЕТЕ"))) || word.endsWith(UniString("ТЬ")))
    {
        if (left_letters > 2)
            result.insert(result.end(), left_letters - 2, PhemTag::END);
        result.push_back(PhemTag::B_SUFF);
        result.insert(result.end(), 1, PhemTag::SUFF);
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
    SuffixDictAnalyzer analyzer;
    std::istream & is = std::cin;
    std::string line;

    std::unordered_set<std::string> unificator;
    std::unordered_set<std::string> abandoned_wf;
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
        if (lexeme[0]->at == AnalyzerTag::SUFF)
        {
            continue;
        }

        //std::cout << curword << '\t' << WordFormPrinter::writePhemInfo(curword, orig_info)  << '\t' << sp << '\t' << "LEMMA" << std::endl;

        for (auto ptr : lexeme)
        {
            std::optional<WordWithInfo> result;
            if (ptr->sp == UniSPTag::NOUN || ptr->sp == UniSPTag::ADJ || ptr->sp == UniSPTag::ADV)
            {
                result = getNounAdjForm(curword.toUpperCase(), ptr->wordform, orig_info, ptr->sp == UniSPTag::ADV, ptr->sp, ptr->mt);
            }
            else if (ptr->sp == UniSPTag::VERB)
            {
                result = getVerbFormV2(curword.toUpperCase(), ptr->wordform, orig_info);
            }

            if (result && validateParse(result->pheminfo))
            {
                auto word = result->word.toLowerCase().replace(u'ё', u'е');
                std::ostringstream oss;
                oss << word << '\t' << WordFormPrinter::writePhemInfo(word, result->pheminfo) << '\t' << ptr->sp << '\t' << "WORDFORM";
                if (!unificator.count(oss.str()))
                {
                    //std::cout << oss.str() << std::endl;
                    unificator.emplace(oss.str());
                }
            }
            else if (!result && ptr->wordform.toLowerCase().replace(u'ё', u'е') != curword && (ptr->sp == UniSPTag::ADJ || ptr->sp == UniSPTag::VERB || ptr->sp == UniSPTag::NOUN || ptr->sp == UniSPTag::ADV))
            {
                auto word = ptr->wordform.toLowerCase().replace(u'ё', u'е');
                std::cout << word << '\t' << ptr->sp << '\t' << ptr->mt << '\t' << curword << '\t' << WordFormPrinter::writePhemInfo(curword, orig_info) << std::endl;
                abandoned_wf.insert(ptr->wordform.toLowerCase().replace(u'ё', u'е').getRawString());
            }
        }
        std::cout << "\n";
    }

    std::cerr << "Abandoned size:" << abandoned_wf.size() << std::endl;
    return 0;
}
