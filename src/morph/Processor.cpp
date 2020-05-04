#include "Processor.h"

namespace X
{

Processor::Processor()
{
    morphAnalyzer = std::make_shared<HyphenAnalyzer>();
}

WordFormPtr Processor::processOneToken(TokenPtr token) const
{
    std::unordered_set<MorphInfo> infos;
    utils::UniString tokenString = token->getInner().toUpperCase().replace(u'Ё', u'Е');
    if (token->getType().contains(TokenTypeTag::WORD | TokenTypeTag::NUMB) && token->getTag() & GraphemTag::CONNECTED)
    {
        parseWordNumLike(infos, tokenString);
    }
    else if (token->getType() & TokenTypeTag::PNCT)
    {
        infos.emplace(MorphInfo{tokenString, UniSPTag::PUNCT, UniMorphTag::UNKN, 1.0, AnalyzerTag::DICT, tokenString.length()});
    }
    else if (token->getType() & TokenTypeTag::NUMB)
    {
        parseNumbLike(infos, tokenString);
    }
    else if (
        (token->getType() & TokenTypeTag::WORD && token->getTag() & GraphemTag::CYRILLIC)
        || (token->getType() & TokenTypeTag::WRNM && token->getTag() & GraphemTag::CYRILLIC))
    {
        parseWordLike(infos, tokenString);
    }
    else if (token->getType() & TokenTypeTag::WORD && token->getTag() & GraphemTag::LATIN)
    {
        infos.emplace(MorphInfo{tokenString, UniSPTag::X, UniMorphTag::UNKN, 1., AnalyzerTag::UNKN, tokenString.length()});
    }
    if (token->getInner().contains(u'_') || token->getInner().contains(u'.'))
    {
        std::vector<MorphInfo> infGood;
        for (auto & info : infos)
        {
            infGood.push_back(info);
            infGood.back().normalForm = token->getInner();
        }
        infos = std::unordered_set<MorphInfo>(infGood.begin(), infGood.end());
    }
    return std::make_shared<WordForm>(token->getInner(), infos, token->getType(), token->getTag());
}

void Processor::parseWordNumLike(std::unordered_set<MorphInfo> & infos, const utils::UniString & tokenString) const
{
    if (morphAnalyzer->isDictWord(tokenString))
    {
        parseWordLike(infos, tokenString);
        return;
    }

    if (tokenString.contains('-'))
    {
        std::vector<utils::UniString> parts = tokenString.split('-');
        if (parts.size() == 2)
        {
            if (!parts[0].isNumber())
            {
                parseWordLike(infos, parts[0], utils::UniString(""), utils::UniString("-") + parts[1]);
            }
            else
            {
                parseWordLike(infos, parts[1], parts[0] + utils::UniString("-"), utils::UniString(""));
            }
        }
    }
    else
    {
        parseWordLike(infos, tokenString);
    }
}

void Processor::parseNumbLike(std::unordered_set<MorphInfo> & infos, const utils::UniString & tokenString) const
{
    infos.insert(MorphInfo{tokenString, UniSPTag::NUM, UniMorphTag::UNKN, 1, AnalyzerTag::DICT, 0});
}

void Processor::parseWordLike(
    std::unordered_set<MorphInfo> & infos,
    const utils::UniString & tokenString,
    const utils::UniString & prefix,
    const utils::UniString & postfix) const
{
    std::vector<ParsedPtr> parsed = morphAnalyzer->analyze(tokenString);
    double totalCount = 0;
    for (auto ptr : parsed)
        totalCount += ptr->count;

    for (auto ptr : parsed)
    {
        MorphInfo mi{prefix + ptr->normalform + postfix, ptr->sp, ptr->mt, ptr->count / totalCount, ptr->at, ptr->stemLen};
        auto miIn = infos.find(mi);
        if (miIn != infos.end())
            miIn->probability += ptr->count / totalCount;
        else
            infos.insert(mi);
    }
}

std::vector<WordFormPtr> Processor::analyze(const std::vector<TokenPtr> & data) const
{
    std::vector<WordFormPtr> result;
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        result.push_back(processOneToken(data[i]));
    }
    return result;
}

WordFormPtr Processor::analyzeSingleToken(TokenPtr data) const
{
    return processOneToken(data);
}

std::vector<WordFormPtr> Processor::synthesize(WordFormPtr form, UniMorphTag t) const
{
    if (form->getType() & TokenTypeTag::WORD && form->getTag() & GraphemTag::CYRILLIC)
    {
        std::vector<WordFormPtr> result;
        for (const auto & mi : form->getMorphInfo())
        {
            std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(form->getWordForm(), mi.tag, t);
            std::map<utils::UniString, std::unordered_set<MorphInfo>> relation;
            for (auto ptr : parsed)
            {
                relation[ptr->wordform].insert(MorphInfo{ptr->normalform, ptr->sp, ptr->mt, 1.0 / parsed.size(), ptr->at, ptr->stemLen});
            }
            for (auto itr : relation)
            {
                result.push_back(std::make_shared<WordForm>(itr.first, itr.second, TokenTypeTag::WORD, GraphemTag::CYRILLIC));
            }
        }
        return result;
    }
    else
    {
        return std::vector<WordFormPtr>();
    }
}

std::vector<WordFormPtr> Processor::synthesize(TokenPtr tok, UniMorphTag t) const
{
    if (tok->getType() & TokenTypeTag::WORD && tok->getTag() & GraphemTag::CYRILLIC)
    {
        return synthesize(tok->getInner(), t);
    }
    else
    {
        return std::vector<WordFormPtr>();
    }
}

std::vector<WordFormPtr> Processor::synthesize(const utils::UniString & word, UniMorphTag t) const
{
    std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(word.toUpperCase().replace(u'Ё', u'Е'), t);
    std::map<utils::UniString, std::unordered_set<MorphInfo>> relation;
    for (auto ptr : parsed)
    {
        relation[ptr->wordform].insert(MorphInfo{ptr->normalform, ptr->sp, ptr->mt, 1.0 / parsed.size(), ptr->at, ptr->stemLen});
    }
    std::vector<WordFormPtr> result;
    for (auto itr : relation)
    {
        result.push_back(std::make_shared<WordForm>(itr.first, itr.second, TokenTypeTag::WORD, GraphemTag::CYRILLIC));
    }
    return result;
}
}
