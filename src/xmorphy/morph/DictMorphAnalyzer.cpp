#include <xmorphy/morph/DictMorphAnalyzer.h>

#include <incbin.h>
namespace X
{

namespace
{
    INCBIN(affixdict, "dicts/udaffixdict.bin");
    INCBIN(maindict, "dicts/udmaindict.bin");
}

DictMorphAnalyzer::DictMorphAnalyzer(std::istream & mainDictIs, std::istream & affixDictIs)
    : dict(MorphDict::loadFromFiles(mainDictIs, affixDictIs))
{
}

DictMorphAnalyzer::DictMorphAnalyzer()
{
    std::istringstream mainIs(std::string{reinterpret_cast<const char *>(gmaindictData), gmaindictSize});
    std::istringstream affixIs(std::string{reinterpret_cast<const char *>(gaffixdictData), gaffixdictSize});
    dict = MorphDict::loadFromFiles(mainIs, affixIs);
}

UniString DictMorphAnalyzer::buildNormalForm(
    UniString wordForm,
    const UniString & formPrefix,
    const UniString & formSuffix,
    UniString normalFormPrefix,
    const UniString & normalFormSuffix) const
{
    if (formPrefix.length() < wordForm.length())
    {
        if (!formPrefix.isEmpty())
        {
            wordForm = wordForm.subString(formPrefix.length());
        }
    }
    else
    {
        normalFormPrefix = UniString("");
    }
    UniString stem = wordForm.subString(0, wordForm.length() - formSuffix.length());
    return normalFormPrefix + stem + normalFormSuffix;
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const UniString & str) const
{
    std::vector<MorphDictInfo> dictInfo = dict->getClearForms(str);
    return analyze(str, dictInfo);
}


std::vector<ParsedPtr> DictMorphAnalyzer::generate(const UniString & str, const std::map<Paradigm, size_t> & paradigms) const
{
    std::vector<ParsedPtr> result;
    for (const auto & para : paradigms)
    {
        const LexemeGroup & lg = para.first[para.second];
        for (const LexemeGroup & group : para.first)
        {
            auto parsed = buildByPara(group, lg, para.first[0], str, true);
            if (parsed)
            {
                result.push_back(parsed);
                result.back()->sp = group.sp;
                result.back()->mt = group.tag;
                result.back()->at = AnalyzerTag::DICT;
            }
        }
    }
    return result;
}

std::vector<ParsedPtr> DictMorphAnalyzer::generate(const UniString & str) const
{
    auto paras = dict->getParadigmsForForm(str);
    return generate(str, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const UniString & str, const std::vector<MorphDictInfo> & dictInfo) const
{
    std::vector<ParsedPtr> result(dictInfo.size());
    std::size_t i = 0;
    for (auto & itr : dictInfo)
    {
        const auto & [prefix, spt, mt, suffix, _] = itr.lexemeGroup;
        const auto & [nprefix, nsuffix] = itr.affixPair;
        UniString normalForm;
        if (UniSPTag::getStaticSPs().count(spt))
            normalForm = str;
        else
            normalForm = buildNormalForm(str, prefix, suffix, nprefix, nsuffix);

        if (spt == UniSPTag::X)
            throw std::runtime_error("Incorrect word in dictionary '" + str.getRawString() + "'");

        result[i] = std::make_shared<Parsed>(
            Parsed{str, normalForm, spt, mt, AnalyzerTag::DICT, itr.occurences, normalForm.length() - nsuffix.length()});
        i++;
    }
    return result;
}

ParsedPtr DictMorphAnalyzer::buildByPara(
    const LexemeGroup & reqForm,
    const LexemeGroup & givenForm,
    const LexemeGroup & normalForm,
    const UniString & given,
    bool only_given_lemma) const
{
    const UniString & prefix = givenForm.prefix;
    const UniString & suffix = givenForm.suffix;
    UniSPTag sp = reqForm.sp;
    UniMorphTag mt = reqForm.tag;
    const UniString & nprefix = normalForm.prefix;
    const UniString & nsuffix = normalForm.suffix;
    const UniString & reqPrefix = reqForm.prefix;
    const UniString & reqSuffix = reqForm.suffix;
    UniString nF = buildNormalForm(given, prefix, suffix, nprefix, nsuffix);
    if (only_given_lemma && nF != given)
        return nullptr;
    UniString f = buildNormalForm(given, prefix, suffix, reqPrefix, reqSuffix);
    return std::make_shared<Parsed>(Parsed{f, nF, sp, mt, AnalyzerTag::DICT, 0, nF.length() - nsuffix.length()});
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const UniString & str, const UniMorphTag & t) const
{
    std::map<Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, t, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(
    const UniString & str,
    const UniMorphTag & t,
    const std::map<Paradigm, std::size_t> & paras) const
{
    std::vector<ParsedPtr> result;
    for (const auto & para : paras)
    {
        const LexemeGroup & given_form = para.first[para.second];
        auto full_required_tag = given_form.tag;
        full_required_tag.setFromTag(t);
        for (const LexemeGroup & group : para.first)
        {
            UniMorphTag current = group.tag;
            /// FIXME
            if (current.contains(full_required_tag))
                result.push_back(buildByPara(group, given_form, para.first[0], str));
        }
    }
    return result;
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    std::map<Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, given, req, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(
    const UniString & str,
    const UniMorphTag & given,
    const UniMorphTag & req,
    const std::map<Paradigm, std::size_t> & paras) const
{
    std::vector<ParsedPtr> result;
    auto given_tag_copy = given;
    given_tag_copy.setFromTag(req);
    for (const auto & para : paras)
    {
        const LexemeGroup & given_form = para.first[para.second];
        for (size_t i = 0; i < para.first.size(); ++i)
        {
            auto form = para.first[i];
            UniMorphTag current = form.tag;
            if (current.contains(given_tag_copy))
                result.push_back(buildByPara(form, given_form, para.first[0], str));
        }
    }
    return result;
}

bool DictMorphAnalyzer::isDictWord(const UniString & str) const
{
    return dict->contains(str);
}

bool DictMorphAnalyzer::isWordContainsInDictionary(const UniString & str) const
{
    return dict->contains(str);
}
}
