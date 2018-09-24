#ifndef _UNI_DEP_IO
#define _UNI_DEP_IO
#include "IIO.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
namespace io {
class UniDepIO : public IIO {
private:
    bool fool;
public:
	static const std::string SAME_FEATURE;
    UniDepIO(bool fool = false) :IIO(), fool(fool) {

    }
	std::string write(analyze::WordFormPtr wform) const override;
    boost::property_tree::ptree writeToJSON(analyze::WordFormPtr wform) const override { return boost::property_tree::ptree(); }
};

}
#endif
