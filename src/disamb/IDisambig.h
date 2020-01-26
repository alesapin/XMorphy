#ifndef _I_DISAMBIG_H
#define _I_DISAMBIG_H
#include <morph/WordForm.h>
#include <vector>
namespace disamb {
class IDisambig {
public:
	virtual void disambiguate(std::vector<analyze::WordFormPtr> &sequence) const = 0;
    virtual ~IDisambig() {
    }
};
}
#endif
