#ifndef _I_IO_H
#define _I_IO_H
#include "../morph/WordForm.h"
namespace io {
class IIO {
public:
	virtual std::string write(analyze::WordFormPtr wform) const = 0;
};
}
#endif
