#pragma once
#include <xmorphy/graphem/Token.h>

namespace X
{
class Tokenizer
{
private:
    size_t cutWord(size_t start, const UniString & str) const;

    size_t cutWordNum(size_t start, const UniString & str) const;

    size_t cutNumber(size_t start, const UniString & str) const;

    size_t cutSeparator(size_t start, const UniString & str) const;

    size_t cutPunct(size_t start, const UniString & str) const;

    size_t cutTrash(size_t start, const UniString & str) const;

    std::shared_ptr<Token> processWord(const UniString & str) const;

    std::shared_ptr<Token> processPunct(const UniString & str) const;

    std::shared_ptr<Token> processNumber(const UniString & number) const;

    std::shared_ptr<Token> processSeparator(const UniString & sep) const;

    std::shared_ptr<Token> processWordNum(const UniString & wn) const;

    std::shared_ptr<Token> processHieroglyph(const UniString & hir) const;

public:
    virtual std::vector<std::shared_ptr<Token>> analyze(const UniString & text) const;
    virtual std::shared_ptr<Token> analyzeSingleWord(const UniString & word) const;
    virtual ~Tokenizer() {}
};
} // namespace tokenize
