#include <xmorphy/morph/HyphenAnalyzer.h>
#include <incbin.h>

namespace X
{
namespace
{
    UniString concatWithHyph(const UniString & first, const UniString & second)
    {
        return first + UniString("-") + second;
    }

    INCBIN(hyphdict, "dicts/hyphdict.txt");
}

HyphenAnalyzer::HyphenAnalyzer()
{
    std::istringstream hyphDictIs(std::string{reinterpret_cast<const char *>(ghyphdictData), ghyphdictSize});

    constParts = loadPrefixDict(hyphDictIs);
}

std::vector<ParsedPtr> HyphenAnalyzer::analyze(const UniString & str) const
{
    if (str.contains(u'-'))
    {
        if (PrefixAnalyzer::isDictWord(str))
        {
            return PrefixAnalyzer::analyze(str);
        }
        std::vector<UniString> words = str.split('-');
        std::vector<ParsedPtr> result;
        if (words.size() == 2)
        {
            result = twoWordsAnalyze(words[0], words[1]);
        }
        else
        {
            return SuffixDictAnalyzer::analyze(str);
        }
        for (ParsedPtr p : result)
        {
            p->at = AnalyzerTag::HYPH;
        }
        return result;
    }
    else
    {
        return SuffixDictAnalyzer::analyze(str);
    }
}

std::vector<ParsedPtr> HyphenAnalyzer::twoWordsAnalyze(const UniString & first, const UniString & second) const
{
    std::vector<ParsedPtr> left, right, result;
    if (constParts.count(first))
    {
        result = SuffixDictAnalyzer::analyze(second);
        for (ParsedPtr p : result)
        {
            p->wordform = concatWithHyph(first, p->wordform);
            p->normalform = concatWithHyph(first, p->normalform);
        }
    }
    else
    {
        left = SuffixDictAnalyzer::analyze(first);
        right = SuffixDictAnalyzer::analyze(second);
        if (left.empty())
        {
            for (ParsedPtr p : right)
            {
                p->wordform = concatWithHyph(first, p->wordform);
                p->normalform = concatWithHyph(first, p->normalform);
            }
            result = right;
        }
        else if (right.empty())
        {
            for (ParsedPtr p : left)
            {
                p->wordform = concatWithHyph(p->wordform, second);
                p->normalform = concatWithHyph(p->normalform, second);
            }
            result = left;
        }
        else
        {
            result = twoParsesAnalyze(left, right);
        }
    }
    return result;
}

std::vector<ParsedPtr> HyphenAnalyzer::twoParsesAnalyze(const std::vector<ParsedPtr> & left, const std::vector<ParsedPtr> & right) const
{
    std::vector<ParsedPtr> result;

    //Assume that left part is non derivative
    int nonDerivativeLeft = -1;

    if (left[0]->wordform == right[0]->wordform)
    {
        nonDerivativeLeft = 0;
    }
    else
    {
        for (std::size_t i = 0; i < left.size(); ++i)
        {
            if (left[i]->sp == UniSPTag::X || UniSPTag::getStaticSPs().count(left[i]->sp))
            {
                nonDerivativeLeft = i;
                break;
            }
        }
    }
    if (nonDerivativeLeft != -1)
    {
        for (ParsedPtr ptr : right)
        {
            ptr->wordform = concatWithHyph(left[nonDerivativeLeft]->wordform, ptr->wordform);
            ptr->normalform = concatWithHyph(left[nonDerivativeLeft]->normalform, ptr->normalform);
            result.push_back(ptr);
        }
        return result;
    }

    // Both parts derivative choose only concerted
    std::set<std::size_t> uniq;
    for (std::size_t i = 0; i < left.size(); ++i)
    {
        for (std::size_t j = 0; j < right.size(); ++j)
        {
            if (left[i]->sp == right[j]->sp && !uniq.count(j))
            {
                right[j]->wordform = concatWithHyph(left[i]->wordform, right[j]->wordform);
                right[j]->normalform = concatWithHyph(left[i]->normalform, right[j]->normalform);
                result.push_back(right[j]);
                uniq.insert(j);
            }
        }
    }
    if (result.empty())
    {
        right[0]->wordform = concatWithHyph(left[0]->wordform, right[0]->wordform);
        right[0]->normalform = concatWithHyph(left[0]->normalform, right[0]->normalform);
        result.push_back(right[0]);
    }
    return result;
}

std::vector<ParsedPtr> HyphenAnalyzer::synthesize(const UniString & str, const UniMorphTag & t) const
{
    return SuffixDictAnalyzer::synthesize(str, t);
}

std::vector<ParsedPtr> HyphenAnalyzer::synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    return SuffixDictAnalyzer::synthesize(str, given, req);
}
}
