//
// Created by alesapin on 08.07.16.
//

#ifndef CROSSTOKENIZER_DEFAULTTOKENIZER_H
#define CROSSTOKENIZER_DEFAULTTOKENIZER_H

#include <base/IAnalyzer.h>
#include "Token.h"

namespace tokenize {

class Tokenizer {

private:
    uint cutWord(uint start, const utils::UniString &str) const;

    uint cutWordNum(uint start, const utils::UniString &str) const;

    uint cutNumber(uint start, const utils::UniString &str) const;

    uint cutSeparator(uint start, const utils::UniString &str) const;

    uint cutPunct(uint start, const utils::UniString &str) const;

    uint cutTrash(uint start, const utils::UniString &str) const;

    std::shared_ptr<base::Token> processWord(const utils::UniString &str) const;

    std::shared_ptr<base::Token> processPunct(const utils::UniString &str) const;

    std::shared_ptr<base::Token> processNumber(const utils::UniString &number) const;

    std::shared_ptr<base::Token> processSeparator(const utils::UniString &sep) const;

    std::shared_ptr<base::Token> processWordNum(const utils::UniString &wn) const;

    std::shared_ptr<base::Token> processHieroglyph(const utils::UniString &hir) const;

public:
    virtual std::vector<std::shared_ptr<base::Token>> analyze(const utils::UniString &text) const;
};
}

#endif //CROSSTOKENIZER_DEFAULTTOKENIZER_H
