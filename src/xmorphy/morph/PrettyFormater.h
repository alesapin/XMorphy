#pragma once
#include <xmorphy/morph/IFormater.h>

namespace X
{
class PrettyFormater : public IFormater
{
    bool with_morphemic_info;
public:
    PrettyFormater(bool with_morphemic_info_)
        : with_morphemic_info(with_morphemic_info_)
    {}

    std::string format(const std::vector<WordFormPtr> & forms) const override;
};

}
