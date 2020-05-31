#pragma once
#include <xmorphy/graphem/Token.h>

namespace X
{
class Tokenizer
{
private:
    size_t cutWord(size_t start, const utils::UniString & str) const;

    size_t cutWordNum(size_t start, const utils::UniString & str) const;

    size_t cutNumber(size_t start, const utils::UniString & str) const;

    size_t cutSeparator(size_t start, const utils::UniString & str) const;

    size_t cutPunct(size_t start, const utils::UniString & str) const;

    size_t cutTrash(size_t start, const utils::UniString & str) const;

    std::shared_ptr<Token> processWord(const utils::UniString & str) const;

    std::shared_ptr<Token> processPunct(const utils::UniString & str) const;

    std::shared_ptr<Token> processNumber(const utils::UniString & number) const;

    std::shared_ptr<Token> processSeparator(const utils::UniString & sep) const;

    std::shared_ptr<Token> processWordNum(const utils::UniString & wn) const;

    std::shared_ptr<Token> processHieroglyph(const utils::UniString & hir) const;

public:
    virtual std::vector<std::shared_ptr<Token>> analyze(const utils::UniString & text) const;
    virtual std::shared_ptr<Token> analyzeSingleWord(const utils::UniString & word) const;
    virtual ~Tokenizer() {}
};
} // namespace tokenize
