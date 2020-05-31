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

std::set<utils::UniString> PrefixAnalyzer::cutPrefix(const utils::UniString & source) const
{
    std::set<utils::UniString> result;
    for (const utils::UniString & pref : prefDict)
    {
        if (pref.length() >= source.length())
            continue;
        bool failed = false;
        for (std::size_t i = 0; i < pref.length(); ++i)
        {
            if (pref[i] != source[i])
            {
                failed = true;
                break;
            }
        }
        if (!failed && DictMorphAnalyzer::isDictWord(source.subString(pref.length())))
        {
            result.insert(pref);
        }
    }
    return result;
}

bool PrefixAnalyzer::isDictWord(const utils::UniString & str) const
{
    return !cutPrefix(str).empty() || DictMorphAnalyzer::isDictWord(str);
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

std::vector<ParsedPtr> PrefixAnalyzer::analyze(const utils::UniString & str) const
{
    std::vector<ParsedPtr> result;
    if (DictMorphAnalyzer::isDictWord(str))
    {
        result = DictMorphAnalyzer::analyze(str);
    }
    std::set<utils::UniString> possiblePrefixes = cutPrefix(str);
    for (const auto & pref : possiblePrefixes)
    {
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
    std::set<utils::UniString> possiblePrefixes = cutPrefix(str);
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
    std::set<utils::UniString> possiblePrefixes = cutPrefix(str);
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
