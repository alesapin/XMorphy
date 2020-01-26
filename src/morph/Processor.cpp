#include "Processor.h"
namespace analyze {

WordFormPtr Processor::processOneToken(base::TokenPtr token) const {
    std::set<MorphInfo> infos;
    utils::UniString tokenString = token->getInner().toUpperCase().replace(utils::UniCharacter::YO, utils::UniCharacter::YE);
    if (token->getType().contains(base::TokenTypeTag::WORD | base::TokenTypeTag::NUMB)) {
        parseWordNumLike(infos, tokenString);
    } else if (token->getType() & base::TokenTypeTag::NUMB) {
        parseNumbLike(infos, tokenString);
    } else if ((token->getType() & base::TokenTypeTag::WORD && token->getTag() & base::GraphemTag::CYRILLIC)
        || (token->getType() & base::TokenTypeTag::WRNM && token->getTag() & base::GraphemTag::CYRILLIC)) {
        parseWordLike(infos, tokenString);
    }
    if (token->getInner().contains(utils::UniCharacter("_")) || token->getInner().contains(utils::UniCharacter("."))) {
        std::vector<MorphInfo> infGood;
        for (auto& info : infos) {
            infGood.push_back(info);
            infGood.back().normalForm = token->getInner();
        }
        infos = std::set<MorphInfo>(infGood.begin(), infGood.end());
    }
    return std::make_shared<WordForm>(token->getInner(), infos, token->getType(), token->getTag());
}

void Processor::parseWordNumLike(std::set<MorphInfo>& infos, const utils::UniString& tokenString) const {
    std::vector<utils::UniString> parts = tokenString.split(utils::UniCharacter("-"));
    if (parts.size() == 2) {
        bool firstWord = false;
        if (!parts[0].isNumber()) {
            firstWord = true;
        }
        if (firstWord) {
            parseWordLike(infos, parts[0], utils::UniString(""), utils::UniString("-") + parts[1]);
        } else {
            parseWordLike(infos, parts[1], parts[0] + utils::UniString("-"), utils::UniString(""));
        }
    } else {
        parseWordLike(infos, tokenString);
    }
}

void Processor::parseNumbLike(std::set<MorphInfo>& infos, const utils::UniString& tokenString) const {
    infos.insert(MorphInfo{tokenString, base::UniSPTag::NUM, base::UniMorphTag::UNKN, 1, base::AnalyzerTag::DICT, 0});
}

void Processor::parseWordLike(
    std::set<MorphInfo>& infos,
    const utils::UniString& tokenString,
    const utils::UniString& prefix,
    const utils::UniString& postfix) const
{
    std::vector<ParsedPtr> parsed = morphAnalyzer->analyze(tokenString);
    double totalCount = 0;
    for (auto ptr : parsed) {
        totalCount += ptr->count;
    }
    for (auto ptr : parsed) {
        MorphInfo mi{prefix + ptr->normalform + postfix, ptr->sp, ptr->mt, ptr->count / totalCount, ptr->at, ptr->stemLen};
        std::set<MorphInfo>::iterator miIn = infos.find(mi);
        if (miIn != infos.end()) {
            miIn->probability += ptr->count / totalCount;
        } else {
            infos.insert(mi);
        }
    }
}

base::TokenPtr Processor::joinHyphenGroup(std::size_t& index, const std::vector<base::TokenPtr>& data) const
{
    std::size_t i = index;
    if (data.size() < 3 || i >= data.size() - 2) {
        index = i + 1;
        return data[i];
    }
    base::GraphemTag rt = base::GraphemTag::MULTI_WORD;
    base::TokenTypeTag rttt = base::TokenTypeTag::UNKN;
    for (; i < data.size() - 2; i += 2) {
        base::TokenPtr current = data[i];
        base::TokenPtr next = data[i + 1];
        base::TokenPtr nextnext = data[i + 2];
        if (next->getType() == base::TokenTypeTag::PNCT && (next->getTag() & (base::GraphemTag::DASH | base::GraphemTag::LOWER_DASH))) {
            if (current->getType() & (base::TokenTypeTag::WORD | base::TokenTypeTag::NUMB) && nextnext->getType() & (base::TokenTypeTag::WORD | base::TokenTypeTag::NUMB)) {
                rt |= current->getTag() | nextnext->getTag();
                rttt |= current->getType() | nextnext->getType();
            } else {
                break;
            }
        } else {
            break;
        }
    }
    if (i == index) {
        index = i + 1;
        return data[i];
    }
    utils::UniString result;
    for (std::size_t j = index; j <= i; ++j) {
        result = result + data[j]->getInner();
    }
    index = i + 1;
    return std::make_shared<base::Token>(result, rttt, rt);
}

std::vector<WordFormPtr> Processor::analyze(const std::vector<base::TokenPtr>& data) const {
    std::vector<WordFormPtr> result;
    for (std::size_t i = 0; i < data.size();) {
        base::TokenPtr tok = joinHyphenGroup(i, data);
        result.push_back(processOneToken(tok));
    }
    return result;
}

WordFormPtr Processor::analyzeSingleToken(base::TokenPtr data) const {
    return processOneToken(data);
}

std::vector<WordFormPtr> Processor::synthesize(WordFormPtr form, base::UniMorphTag t) const {
    if (form->getType() & base::TokenTypeTag::WORD && form->getTag() & base::GraphemTag::CYRILLIC) {
        std::vector<WordFormPtr> result;
        for (const auto& mi : form->getMorphInfo()) {
            std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(form->getWordForm(), mi.tag, t);
            std::map<utils::UniString, std::set<MorphInfo>> relation;
            for (auto ptr : parsed) {
                relation[ptr->wordform].insert(MorphInfo{ptr->normalform, ptr->sp, ptr->mt, 1.0 / parsed.size(), ptr->at, ptr->stemLen});
            }
            for (auto itr : relation) {
                result.push_back(std::make_shared<WordForm>(itr.first, itr.second, base::TokenTypeTag::WORD, base::GraphemTag::CYRILLIC));
            }
        }
        return result;
    } else {
        return std::vector<WordFormPtr>();
    }
}

std::vector<WordFormPtr> Processor::synthesize(base::TokenPtr tok, base::UniMorphTag t) const {
    if (tok->getType() & base::TokenTypeTag::WORD && tok->getTag() & base::GraphemTag::CYRILLIC) {
        return synthesize(tok->getInner(), t);
    } else {
        return std::vector<WordFormPtr>();
    }
}

std::vector<WordFormPtr> Processor::synthesize(const utils::UniString& word, base::UniMorphTag t) const {
    std::vector<ParsedPtr> parsed = morphAnalyzer->synthesize(word.toUpperCase().replace(utils::UniCharacter::YO, utils::UniCharacter::YE), t);
    std::map<utils::UniString, std::set<MorphInfo>> relation;
    for (auto ptr : parsed) {
        relation[ptr->wordform].insert(MorphInfo{ptr->normalform, ptr->sp, ptr->mt, 1.0 / parsed.size(), ptr->at, ptr->stemLen});
    }
    std::vector<WordFormPtr> result;
    for (auto itr : relation) {
        result.push_back(std::make_shared<WordForm>(itr.first, itr.second, base::TokenTypeTag::WORD, base::GraphemTag::CYRILLIC));
    }
    return result;
}
}
