#include "HyphenAnalyzer.h"
namespace analyze {
namespace {
utils::UniString concatWithDefis(const utils::UniString& first, const utils::UniString& second) {
    return first + utils::UniString("-") + second;
}
}

std::vector<ParsedPtr> HyphenAnalyzer::analyze(const utils::UniString& str) const {
    if (str.contains(utils::UniCharacter("-"))) {
        if (PrefixAnalyzer::isDictWord(str)) {
            return PrefixAnalyzer::analyze(str);
        }
        std::vector<utils::UniString> words = str.split(utils::UniCharacter("-"));
        std::vector<ParsedPtr> result;
        if (words.size() == 2) {
            result = twoWordsAnalyze(words[0], words[1]);
        } else {
            return SuffixDictAnalyzer::analyze(str);
        }
        for (ParsedPtr p : result) {
            p->at = base::AnalyzerTag::HYPH;
        }
        return result;
    } else {
        return SuffixDictAnalyzer::analyze(str);
    }
}

std::vector<ParsedPtr> HyphenAnalyzer::twoWordsAnalyze(const utils::UniString& first, const utils::UniString& second) const {
    std::vector<ParsedPtr> left, right, result;
    if (constParts.count(first)) {
        result = SuffixDictAnalyzer::analyze(second);
        for (ParsedPtr p : result) {
            p->wordform = concatWithDefis(first, p->wordform);
            p->normalform = concatWithDefis(first, p->normalform);
        }
    } else {
        left = SuffixDictAnalyzer::analyze(first);
        right = SuffixDictAnalyzer::analyze(second);
        if (left.empty()) {
            for (ParsedPtr p : right) {
                p->wordform = concatWithDefis(first, p->wordform);
                p->normalform = concatWithDefis(first, p->normalform);
            }
            result = right;
        } else if (right.empty()) {
            for (ParsedPtr p : left) {
                p->wordform = concatWithDefis(p->wordform, second);
                p->normalform = concatWithDefis(p->normalform, second);
            }
            result = left;
        } else {
            result = twoParsesAnalyze(left, right);
        }
    }
    return result;
}

std::vector<ParsedPtr> HyphenAnalyzer::twoParsesAnalyze(const std::vector<ParsedPtr>& left, const std::vector<ParsedPtr>& right) const {
    std::vector<ParsedPtr> result;

    //Assume that left part is non derivative
    int nonDerivativeLeft = -1;

    if (left[0]->wordform == right[0]->wordform) {
        nonDerivativeLeft = 0;
    } else {
        for (std::size_t i = 0; i < left.size(); ++i) {
            if (left[i]->sp == base::UniSPTag::X || base::FIXED_UNISPS.count(left[i]->sp) || left[i]->wordform == left[i]->normalform) {
                nonDerivativeLeft = i;
                break;
            }
        }
    }
    if (nonDerivativeLeft != -1) {
        for (ParsedPtr ptr : right) {
            ptr->wordform = concatWithDefis(left[nonDerivativeLeft]->wordform, ptr->wordform);
            ptr->normalform = concatWithDefis(left[nonDerivativeLeft]->normalform, ptr->normalform);
            result.push_back(ptr);
        }
        return result;
    }

    // Both parts derivative choose only concerted
    std::set<std::size_t> uniq;
    for (std::size_t i = 0; i < left.size(); ++i) {
        for (std::size_t j = 0; j < right.size(); ++j) {
            if (left[i]->sp == right[j]->sp && !uniq.count(j)) { //TODO May be smth better?
                right[j]->wordform = concatWithDefis(left[i]->wordform, right[j]->wordform);
                right[j]->normalform = concatWithDefis(left[i]->normalform, right[j]->normalform);
                result.push_back(right[j]);
                uniq.insert(j);
            }
        }
    }
    if (result.empty()) {
        right[0]->wordform = concatWithDefis(left[0]->wordform, right[0]->wordform);
        right[0]->normalform = concatWithDefis(left[0]->normalform, right[0]->normalform);
        result.push_back(right[0]);
    }
    return result;
}

std::vector<ParsedPtr> HyphenAnalyzer::synthesize(const utils::UniString& str, const base::UniMorphTag& t) const {
    return SuffixDictAnalyzer::synthesize(str, t);
}

std::vector<ParsedPtr> HyphenAnalyzer::synthesize(const utils::UniString& str, const base::UniMorphTag& given, const base::UniMorphTag& req) const {
    return SuffixDictAnalyzer::synthesize(str, given, req);
}
}
