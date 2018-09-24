#include <stdint.h>
#include <string>
#include <memory>                       // for shared_ptr<>
#include <iostream>
#include <deque>
#include <map>
#include <algorithm>                    // for lower_bound()
#include "UniString.h"
#include "UniCharacter.h"
namespace utils {
using namespace std;
class LCS {
protected:
  class Pair {
  public:
    uint32_t index1;
    uint32_t index2;
    shared_ptr<Pair> next;

    Pair(uint32_t index1, uint32_t index2, shared_ptr<Pair> next = nullptr)
      : index1(index1), index2(index2), next(next) {
    }

    static shared_ptr<Pair> Reverse(const shared_ptr<Pair> pairs) {
      shared_ptr<Pair> head = nullptr;
      for (auto next = pairs; next != nullptr; next = next->next)
        head = make_shared<Pair>(next->index1, next->index2, head);
      return head;
    }
  };

  typedef deque<shared_ptr<Pair>> PAIRS;
  typedef deque<uint32_t> THRESHOLD;
  typedef deque<uint32_t> INDEXES;
  typedef map<UniCharacter, INDEXES> CHAR2INDEXES;
  typedef deque<INDEXES*> MATCHES;

  uint64_t Pairs(MATCHES& matches, shared_ptr<Pair> *pairs) {
    auto trace = pairs != nullptr;
    PAIRS traces;
    THRESHOLD threshold;

    uint32_t index1 = 0;
    for (const auto& it1 : matches) {
      if (!it1->empty()) {
        auto dq2 = *it1;
        auto limit = threshold.end();
        for (auto it2 = dq2.begin(); it2 != dq2.end(); it2++) {
          auto index2 = *it2;


          limit = lower_bound(threshold.begin(), limit, index2);
          auto index3 = distance(threshold.begin(), limit);
          auto skip = it2 + 1 != dq2.end() &&
            (limit == threshold.begin() || *(limit - 1) < *(it2 + 1));

          if (skip) continue;

          if (limit == threshold.end()) {
            threshold.push_back(index2);
            if (trace) {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last = make_shared<Pair>(index1, index2, prefix);
              traces.push_back(last);
            }
          }
          else if (index2 < *limit) {
            *limit = index2;
            if (trace) {
              auto prefix = index3 > 0 ? traces[index3 - 1] : nullptr;
              auto last = make_shared<Pair>(index1, index2, prefix);
              traces[index3] = last;
            }
          }
        }
      }

      index1++;
    }

    if (trace) {
      auto last = traces.size() > 0 ? traces.back() : nullptr;
      *pairs = Pair::Reverse(last);
    }

    auto length = threshold.size();
    return length;
  }

  void Match(CHAR2INDEXES& indexes, MATCHES& matches, const UniString& s1, const UniString& s2) {
    uint32_t index = 0;
    for (const auto& it : s2)
      indexes[it].push_front(index++);

    for (const auto& it : s1) {
      auto& dq2 = indexes[it];
      matches.push_back(&dq2);
    }
  }

  UniString Select(shared_ptr<Pair> pairs, uint64_t length, bool right, const UniString& s1, const UniString& s2) {
    string buffer;
    buffer.reserve(length);
    for (auto next = pairs; next != nullptr; next = next->next) {
      auto c = right ? s2[next->index2] : s1[next->index1];
      buffer += c.getInnerRepr();
    }
    return UniString(buffer, s1.getLocale());
  }

public:
  UniString Correspondence(const UniString& s1, const UniString& s2) {
    CHAR2INDEXES indexes;
    MATCHES matches;                    // holds references into indexes
    Match(indexes, matches, s1, s2);
    shared_ptr<Pair> pairs;             // obtain the LCS as index pairs
    auto length = Pairs(matches, &pairs);
    return Select(pairs, length, false, s1, s2);
  }
};
}
