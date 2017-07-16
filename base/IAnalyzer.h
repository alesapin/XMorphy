#ifndef I_ANALYZER_H
#define I_ANALYZER_H
#include <vector>
#include <memory>
#include "IEntity.h"
namespace base {
class IAnalyzer {
public:
    virtual ~IAnalyzer() = 0;
    virtual std::vector<std::shared_ptr<IEntity>> analyze(const std::vector<std::shared_ptr<IEntity>> &data) = 0;
};
inline IAnalyzer::~IAnalyzer() {};
}
#endif
