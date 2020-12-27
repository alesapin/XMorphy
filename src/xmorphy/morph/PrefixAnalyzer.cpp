#include <xmorphy/morph/PrefixAnalyzer.h>
#include <incbin.h>

namespace X
{
namespace
{
    INCBIN(prefixdict, "dicts/prefixdict.txt");
}

PrefixAnalyzer::PrefixAnalyzer()
{
    std::istringstream prefixDictIs(std::string{reinterpret_cast<const char *>(gprefixdictData), gprefixdictSize});
    prefDict = loadPrefixDict(prefixDictIs);
}

std::unordered_set<utils::UniString> PrefixAnalyzer::cutPrefix(const utils::UniString & source) const
{
    std::unordered_set<utils::UniString> result;
    for (const utils::UniString & pref : prefDict)
    {
        if (pref.length() >= source.length())
            continue;

        bool failed = false;

        for (size_t i = 0, size = pref.length(); i < size; ++i)
        {
            if (pref[i] != source[i])
            {
                failed = true;
                break;
            }
        }

        if (!failed && DictMorphAnalyzer::isDictWord(source.subString(pref.length())))
            result.insert(pref);
    }
    return result;
}

bool PrefixAnalyzer::isDictWord(const utils::UniString & str) const
{
    if (DictMorphAnalyzer::isDictWord(str))
        return true;
    auto possible_prefixes = cutPrefix(str);
    if (possible_prefixes.empty())
        return false;
    for (const auto & pref : possible_prefixes)
        if (pref.length() < str.length() && str.length() - pref.length() >= 3)
            return true;
    return false;
}

namespace
{
    void mergePrefix(std::vector<ParsedPtr> & ptr, const utils::UniString & prefix)
    {
        for (ParsedPtr p : ptr)
        {
            p->normalform = prefix + p->normalform;
            p->wordform = prefix + p->wordform;
            p->at = AnalyzerTag::PREF;
        }
    }
}

std::vector<ParsedPtr> PrefixAnalyzer::generate(const utils::UniString & str) const
{
    if (DictMorphAnalyzer::isDictWord(str))
        return DictMorphAnalyzer::generate(str);

    std::unordered_set<utils::UniString> possiblePrefixes = cutPrefix(str);
    std::vector<ParsedPtr> result;

    for (const auto & pref : possiblePrefixes)
    {
        if (pref.length() >= str.length() || str.length() - pref.length() < 3)
            continue;

        std::vector<ParsedPtr> current = DictMorphAnalyzer::generate(str.subString(pref.length()));
        mergePrefix(current, pref);
        result.insert(result.end(), current.begin(), current.end());
    }

    return result;
}

std::vector<ParsedPtr> PrefixAnalyzer::analyze(const utils::UniString & str) const
{
    std::vector<ParsedPtr> result;
    if (DictMorphAnalyzer::isDictWord(str))
        result = DictMorphAnalyzer::analyze(str);

    std::unordered_set<utils::UniString> possiblePrefixes = cutPrefix(str);
    for (const auto & pref : possiblePrefixes)
    {
        if (pref.length() >= str.length() || str.length() - pref.length() < 3)
            continue;

        std::vector<ParsedPtr> current = DictMorphAnalyzer::analyze(str.subString(pref.length()));
        mergePrefix(current, pref);
        result.insert(result.end(), current.begin(), current.end());
    }
    return result;
}

std::vector<ParsedPtr> PrefixAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & t) const
{
    if (DictMorphAnalyzer::isDictWord(str))
    {
        return DictMorphAnalyzer::synthesize(str, t);
    }
    std::unordered_set<utils::UniString> possiblePrefixes = cutPrefix(str);
    std::vector<ParsedPtr> result;
    for (const auto & pref : possiblePrefixes)
    {
        std::vector<ParsedPtr> current = DictMorphAnalyzer::synthesize(str.cut(pref.length()), t);
        mergePrefix(current, pref);
        result.insert(result.end(), current.begin(), current.end());
    }
    return result;
}

std::vector<ParsedPtr> PrefixAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    if (DictMorphAnalyzer::isDictWord(str))
    {
        return DictMorphAnalyzer::synthesize(str, given, req);
    }
    std::unordered_set<utils::UniString> possiblePrefixes = cutPrefix(str);
    std::vector<ParsedPtr> result;
    for (const auto & pref : possiblePrefixes)
    {
        std::vector<ParsedPtr> current = DictMorphAnalyzer::synthesize(str.cut(pref.length()), given, req);
        mergePrefix(current, pref);
        result.insert(result.end(), current.begin(), current.end());
    }
    return result;
}
}
