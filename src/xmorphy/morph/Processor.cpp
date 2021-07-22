#include <xmorphy/morph/Processor.h>

namespace X
{

Processor::Processor()
    : forms_cache(5000)
{
    morphAnalyzer = std::make_shared<HyphenAnalyzer>();
}

WordFormPtr Processor::processOneToken(TokenPtr token) const
{
    std::unordered_set<MorphInfo> infos;
    UniString tokenString = token->getInner().toUpperCase().replace(u'Ё', u'Е');

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
        parseNumbLike(infos, std::move(tokenString));
    }
    else if (token->getType() & TokenTypeTag::WORD && token->getTag() & GraphemTag::CYRILLIC)
    {
        parseWordLike(infos, tokenString);
    }
    else if (token->getType() & TokenTypeTag::WRNM && token->getTag() & GraphemTag::CYRILLIC)
    {
        parseWordLike(infos, tokenString);
        if (infos.empty())
            infos.emplace(MorphInfo(std::move(tokenString), UniSPTag::X, UniMorphTag::UNKN, 1., AnalyzerTag::UNKN, tokenString.length()));
    }
    else if (token->getType() & TokenTypeTag::WORD && token->getTag() & GraphemTag::LATIN)
    {
        infos.emplace(MorphInfo(std::move(tokenString), UniSPTag::X, UniMorphTag::UNKN, 1., AnalyzerTag::UNKN, tokenString.length()));
    }

    if (infos.empty())
    {
        infos.emplace(MorphInfo(std::move(tokenString), UniSPTag::X, UniMorphTag::UNKN, 1., AnalyzerTag::UNKN, tokenString.length()));
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
    return std::make_shared<WordForm>(std::move(*token), std::move(infos));
}

void Processor::parseWordNumLike(std::unordered_set<MorphInfo> & infos, const UniString & tokenString) const
{
    if (morphAnalyzer->isDictWord(tokenString))
    {
        parseWordLike(infos, tokenString);
        return;
    }

    if (tokenString.contains('-'))
    {
        std::vector<UniString> parts = tokenString.split('-');
        if (parts.size() == 2)
        {
            if (!parts[0].isNumber())
            {
                parseWordLike(infos, parts[0], UniString(""), UniString("-") + parts[1]);
            }
            else
            {
                parseWordLike(infos, parts[1], parts[0] + UniString("-"), UniString(""));
            }
        }
    }
    else
    {
        parseWordLike(infos, tokenString);
    }
}

void Processor::parseNumbLike(std::unordered_set<MorphInfo> & infos, UniString && tokenString) const
{
    infos.insert(MorphInfo{std::move(tokenString), UniSPTag::NUM, UniMorphTag::UNKN, 1, AnalyzerTag::DICT, 0});
}

void Processor::parseWordLike(
    std::unordered_set<MorphInfo> & infos,
    const UniString & tokenString) const
{
    std::vector<ParsedPtr> parsed = morphAnalyzer->analyze(tokenString);
    double totalCount = 0;
    for (auto ptr : parsed)
        totalCount += ptr->count;

    for (auto ptr : parsed)
    {
        MorphInfo mi(
            std::move(ptr->normalform), ptr->sp, ptr->mt,
            ptr->count / totalCount, ptr->at, ptr->stemLen);
        auto miIn = infos.find(mi);
        if (miIn != infos.end())
            miIn->probability += ptr->count / totalCount;
        else
            infos.insert(mi);
    }
}

void Processor::parseWordLike(
    std::unordered_set<MorphInfo> & infos,
    const UniString & tokenString,
    const UniString & prefix,
    const UniString & postfix) const
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
        result.push_back(analyzeSingleToken(data[i]));
    }
    return result;
}

WordFormPtr Processor::analyzeSingleToken(TokenPtr data) const
{
    WordFormPtr result;

    if (forms_cache.exists(data->getInner()))
    {
        result = std::make_shared<WordForm>(forms_cache.get(data->getInner()));
    }
    else
    {
        result = processOneToken(data);
        forms_cache.put(data->getInner(), *result);
    }
    return result;
}

std::vector<WordFormPtr> Processor::synthesize(WordFormPtr form, UniMorphTag t) const
{
    if (form->getTokenType() & TokenTypeTag::WORD && form->getGraphemTag() & GraphemTag::CYRILLIC)
    {
        std::vector<WordFormPtr> result;
        for (const auto & mi : form->getMorphInfo())
        {
            std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(form->getWordForm(), mi.tag, t);
            std::map<UniString, std::unordered_set<MorphInfo>> relation;
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

std::vector<WordFormPtr> Processor::synthesize(const UniString & word, UniMorphTag t) const
{
    std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(word.toUpperCase(), t);
    std::map<UniString, std::unordered_set<MorphInfo>> relation;
    for (auto ptr : parsed)
    {
        relation[ptr->wordform].insert(MorphInfo{ptr->normalform, ptr->sp, ptr->mt, 1.0 / parsed.size(), ptr->at, ptr->stemLen});
    }
    std::vector<WordFormPtr> result;
    for (auto ptr : parsed)
    {
        auto it = relation.find(ptr->wordform);
        if (it != relation.end())
        {
            auto infos = relation[ptr->wordform];
            result.push_back(std::make_shared<WordForm>(ptr->wordform, infos, TokenTypeTag::WORD, GraphemTag::CYRILLIC));
            relation.erase(it);
        }
    }
    return result;
}

std::vector<TokenPtr> Processor::getNonDictionaryWords(const std::vector<TokenPtr> & data) const
{
    std::vector<TokenPtr> result;
    for (const auto & token : data)
    {
        if (token->getType() == TokenTypeTag::WORD && token->getTag().contains(GraphemTag::CYRILLIC))
        {
            std::vector<TokenPtr> subtokens;
            const auto & token_str = token->getInner();
            size_t prev = 0;
            for (size_t i = 1; i < token_str.length(); ++i)
            {
                if (X::isupper(token_str[i]))
                {

                    auto new_token = std::make_shared<Token>(token_str.subString(prev, prev - i), token->getType(), token->getTag());
                    subtokens.emplace_back(new_token);
                    prev = i;
                }
            }
            auto new_token = std::make_shared<Token>(token_str.subString(prev), token->getType(), token->getTag());
            subtokens.emplace_back(new_token);

            for (const auto & subtoken : subtokens)
            {
                auto up_word = subtoken->getInner().toUpperCase();
                bool found = true;
                if (!isWordContainsInDictionary(up_word))
                {
                    found = false;
                    for (size_t i = 0; i < up_word.length(); ++i)
                    {
                        if (up_word[i] == u'Е')
                        {
                            up_word.replaceInPlace(i, u'Ё');
                            found = isWordContainsInDictionary(up_word);
                            up_word.replaceInPlace(i, u'Е');
                        }
                        else if (up_word[i] == u'Ё')
                        {
                            up_word.replaceInPlace(i, u'Е');
                            found = isWordContainsInDictionary(up_word);
                            up_word.replaceInPlace(i, u'Ё');
                        }
                        if (found)
                            break;
                    }
                }

                if (!found)
                    result.push_back(subtoken);
            }
        }
    }
    return result;
}


bool Processor::isWordContainsInDictionary(const UniString & word) const
{
    return morphAnalyzer->isWordContainsInDictionary(word);
}

std::vector<ParsedPtr> Processor::generate(const UniString & str) const
{
    return morphAnalyzer->generate(str);
}

}
