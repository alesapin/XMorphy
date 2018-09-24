#ifndef _OP_CORPORA_H
#define _OP_CORPORA_H
#include "IIO.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
namespace io {
class OpCorporaIO : public IIO {
public:
	static const std::string SAME_FEATURE;
	using IIO::IIO;
	std::string write(analyze::WordFormPtr wform) const override;
    boost::property_tree::ptree writeToJSON(analyze::WordFormPtr wform) const override;
};
}
#endif
