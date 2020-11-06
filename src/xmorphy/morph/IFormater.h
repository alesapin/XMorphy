#pragma once
#include <string>
#include <vector>
#include <memory>
#include <xmorphy/morph/WordForm.h>

namespace X
{
class IFormater
{
public:
    virtual std::string format(const std::vector<WordFormPtr> & forms) const = 0;
    virtual ~IFormater() {}
};

using FormaterPtr = std::unique_ptr<IFormater>;

   
}
