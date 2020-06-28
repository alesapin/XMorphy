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

utils::UniString DictMorphAnalyzer::buildNormalForm(
    utils::UniString wordForm,
    const utils::UniString & formPrefix,
    const utils::UniString & formSuffix,
    utils::UniString normalFormPrefix,
    const utils::UniString & normalFormSuffix) const
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
        normalFormPrefix = utils::UniString("");
    }
    utils::UniString stem = wordForm.subString(0, wordForm.length() - formSuffix.length());
    return normalFormPrefix + stem + normalFormSuffix;
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const utils::UniString & str) const
{
    std::vector<MorphDictInfo> dictInfo = dict->getClearForms(str);
    return analyze(str, dictInfo);
}

std::vector<ParsedPtr> DictMorphAnalyzer::generate(const utils::UniString & str) const
{
    auto paras = dict->getParadigmsForForm(str);
    std::vector<ParsedPtr> result;
    for (const auto & para : paras)
    {
        LexemeGroup lg = para.first[para.second];
        for (const LexemeGroup & group : para.first)
        {
            result.push_back(buildByPara(group, lg, para.first[0], str));
            result.back()->sp = group.sp;
            result.back()->mt = group.tag;
        }
    }
    return result;
}

std::vector<ParsedPtr> DictMorphAnalyzer::analyze(const utils::UniString & str, const std::vector<MorphDictInfo> & dictInfo) const
{
    std::vector<ParsedPtr> result(dictInfo.size());
    std::size_t i = 0;
    for (auto & itr : dictInfo)
    {
        const auto & [prefix, spt, mt, suffix, _] = itr.lexemeGroup;
        const auto & [nprefix, nsuffix] = itr.affixPair;
        utils::UniString normalForm;
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
    const utils::UniString & given) const
{
    const utils::UniString & prefix = givenForm.prefix;
    const utils::UniString & suffix = givenForm.suffix;
    UniSPTag sp = reqForm.sp;
    UniMorphTag mt = reqForm.tag;
    const utils::UniString & nprefix = normalForm.prefix;
    const utils::UniString & nsuffix = normalForm.suffix;
    const utils::UniString & reqPrefix = reqForm.prefix;
    const utils::UniString & reqSuffix = reqForm.suffix;
    utils::UniString nF = buildNormalForm(given, prefix, suffix, nprefix, nsuffix);
    utils::UniString f = buildNormalForm(given, prefix, suffix, reqPrefix, reqSuffix);
    return std::make_shared<Parsed>(Parsed{f, nF, sp, mt, AnalyzerTag::DICT, 0, nF.length() - nsuffix.length()});
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & t) const
{
    std::map<Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, t, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(
    const utils::UniString & str,
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
            if (current.contains(full_required_tag))
                result.push_back(buildByPara(group, given_form, para.first[0], str));
        }
    }
    return result;
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(const utils::UniString & str, const UniMorphTag & given, const UniMorphTag & req) const
{
    std::map<Paradigm, std::size_t> paras = dict->getParadigmsForForm(str);
    return synthesize(str, given, req, paras);
}

std::vector<ParsedPtr> DictMorphAnalyzer::synthesize(
    const utils::UniString & str,
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

bool DictMorphAnalyzer::isDictWord(const utils::UniString & str) const
{
    return dict->contains(str);
}

bool DictMorphAnalyzer::isWordContainsInDictionary(const utils::UniString & str) const
{
    return dict->contains(str);
}
}
