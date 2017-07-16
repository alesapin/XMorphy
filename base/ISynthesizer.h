#ifndef _I_SYNTHESIZER_H
#define _I_SYNTHESIZER_H
#include <vector>
#include <memory>
#include "IEntity.h"
namespace base {
class ISynthesizer {
public:
    virtual ~ISynthesizer() = 0;
    virtual std::vector<std::shared_ptr<IEntity>> synthesize(const std::vector<std::shared_ptr<IEntity>> &data) = 0;
};
inline ISynthesizer::~ISynthesizer() {};
}

#endif
