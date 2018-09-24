#ifndef _I_IO_H
#define _I_IO_H
#include <morph/WordForm.h>
#include <boost/property_tree/ptree.hpp>
namespace io {
class IIO {
public:
	virtual std::string write(analyze::WordFormPtr wform) const = 0;
	virtual boost::property_tree::ptree writeToJSON(analyze::WordFormPtr wform) const = 0;
};
}
#endif
