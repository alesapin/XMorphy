#include "Processor.h"
#include <incbin.h>

namespace X
{
namespace
{
    INCBIN(hyphdict, "dicts/hyphdict.txt");
    INCBIN(prefixdict, "dicts/prefixdict.txt");
    INCBIN(affixdict, "dicts/udaffixdict.bin");
    INCBIN(maindict, "dicts/udmaindict.bin");
    INCBIN(suffixdict, "dicts/udsuffixdict.bin");
} // namespace

Processor::Processor()
{
    std::istringstream mainIs(std::string{reinterpret_cast<const char *>(gmaindictData), gmaindictSize});
    std::istringstream affixIs(std::string{reinterpret_cast<const char *>(gaffixdictData), gaffixdictSize});
    std::istringstream prefixDict(std::string{reinterpret_cast<const char *>(gprefixdictData), gprefixdictSize});
    std::istringstream suffixDict(std::string{reinterpret_cast<const char *>(gsuffixdictData), gsuffixdictSize});
    std::istringstream hyphDict(std::string{reinterpret_cast<const char *>(ghyphdictData), ghyphdictSize});

    morphAnalyzer = std::make_shared<HyphenAnalyzer>(mainIs, affixIs, prefixDict, suffixDict, hyphDict);
}

WordFormPtr Processor::processOneToken(TokenPtr token) const
{
    std::unordered_set<MorphInfo> infos;
    utils::UniString tokenString = token->getInner().toUpperCase().replace(u'ё', u'е');
    if (token->getType().contains(TokenTypeTag::WORD | TokenTypeTag::NUMB))
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
    std::vector<utils::UniString> parts = tokenString.split('-');
    if (parts.size() == 2)
    {
        bool firstWord = false;
        if (!parts[0].isNumber())
        {
            firstWord = true;
        }
        if (firstWord)
        {
            parseWordLike(infos, parts[0], utils::UniString(""), utils::UniString("-") + parts[1]);
        }
        else
        {
            parseWordLike(infos, parts[1], parts[0] + utils::UniString("-"), utils::UniString(""));
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
    {
        totalCount += ptr->count;
    }

    for (auto ptr : parsed)
    {
        MorphInfo mi{prefix + ptr->normalform + postfix, ptr->sp, ptr->mt, ptr->count / totalCount, ptr->at, ptr->stemLen};
        auto miIn = infos.find(mi);
        if (miIn != infos.end())
        {
            miIn->probability += ptr->count / totalCount;
        }
        else
        {
            infos.insert(mi);
        }
    }
}

TokenPtr Processor::joinHyphenGroup(std::size_t & index, const std::vector<TokenPtr> & data) const
{
    std::size_t i = index;
    if (data.size() < 3 || i >= data.size() - 2)
    {
        index = i + 1;
        return data[i];
    }
    GraphemTag rt = GraphemTag::MULTI_WORD;
    TokenTypeTag rttt = TokenTypeTag::UNKN;
    for (; i < data.size() - 2; i += 2)
    {
        TokenPtr current = data[i];
        TokenPtr next = data[i + 1];
        TokenPtr nextnext = data[i + 2];
        if (next->getType() == TokenTypeTag::PNCT && (next->getTag() & (GraphemTag::DASH | GraphemTag::LOWER_DASH)))
        {
            if (current->getType() & (TokenTypeTag::WORD | TokenTypeTag::NUMB)
                && nextnext->getType() & (TokenTypeTag::WORD | TokenTypeTag::NUMB))
            {
                rt |= current->getTag() | nextnext->getTag();
                rttt |= current->getType() | nextnext->getType();
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    if (i == index)
    {
        index = i + 1;
        return data[i];
    }
    utils::UniString result;
    for (std::size_t j = index; j <= i; ++j)
    {
        result = result + data[j]->getInner();
    }
    index = i + 1;
    return std::make_shared<Token>(result, rttt, rt);
}

std::vector<WordFormPtr> Processor::analyze(const std::vector<TokenPtr> & data) const
{
    std::vector<WordFormPtr> result;
    for (std::size_t i = 0; i < data.size();)
    {
        TokenPtr tok = joinHyphenGroup(i, data);
        result.push_back(processOneToken(tok));
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
