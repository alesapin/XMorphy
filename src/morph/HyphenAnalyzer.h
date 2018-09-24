#ifndef _HYPHEN_ANALYZER_H
#define _HYPHEN_ANALYZER_H
#include <build/DictBuilder.h>
#include "SuffixDictAnalyzer.h"
namespace analyze {
class HyphenAnalyzer : public SuffixDictAnalyzer {
private:
  std::vector<ParsedPtr>
  twoParsesAnalyze(const std::vector<ParsedPtr> &left,
                   const std::vector<ParsedPtr> &right) const;
  std::vector<ParsedPtr> twoWordsAnalyze(const utils::UniString &first,
                                         const utils::UniString &second) const;
  std::set<utils::UniString> constParts;

public:
  HyphenAnalyzer(const std::string &mainDictPath,
                 const std::string &affixDictPath,
                 const std::string &prefixDictPath,
                 const std::string &suffixDictPath,
                 const std::string &hyphenDictPath)
      : SuffixDictAnalyzer(mainDictPath, affixDictPath, prefixDictPath,
                           suffixDictPath) {
    build::loadRealPrefixDict(constParts, hyphenDictPath);
  }
  using SuffixDictAnalyzer::isDictWord;
  std::vector<ParsedPtr> analyze(const utils::UniString &str) const override;
  std::vector<ParsedPtr> synthesize(const utils::UniString &str,
                                    const base::MorphTag &t) const override;
  std::vector<ParsedPtr> synthesize(const utils::UniString &str,
                                    const base::MorphTag &given,
                                    const base::MorphTag &req) const override;
};
} // namespace analyze
#endif
