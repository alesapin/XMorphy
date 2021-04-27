#pragma once
#include <memory>
#include <vector>
#include <xmorphy/tag/AnalyzerTag.h>
#include <xmorphy/tag/UniMorphTag.h>
#include <xmorphy/tag/UniSPTag.h>
#include <xmorphy/utils/UniString.h>

namespace X
{
struct Parsed
{
    UniString wordform;
    UniString normalform;
    UniSPTag sp;
    UniMorphTag mt;
    AnalyzerTag at;
    std::size_t count;
    std::size_t stemLen;
};

using ParsedPtr = std::shared_ptr<Parsed>;

class IMorphAnalyzer
{
public:
    virtual std::vector<ParsedPtr> analyze(const UniString & str) const = 0;
    virtual std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & t) const = 0;
    virtual std::vector<ParsedPtr> synthesize(const UniString & str, const UniMorphTag & given, const UniMorphTag & req) const = 0;
    virtual bool isDictWord(const UniString & str) const = 0;
    virtual ~IMorphAnalyzer() {}
};

}
