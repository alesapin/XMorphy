#ifndef ISERIALIAZABLE_H
#define ISERIALIAZABLE_H
#include <cstddef>
#include <ostream>
#include <istream>
namespace dawg{
/**
 * @brief The ISerializable class represent interface
 * that must be implemented by objects, stored in dawg
 */
class ISerializable {
public:
    /**
     * @brief serialize - dump objects to by stream
     * @param os - byte ostream
     * @return success of operation
     */
    virtual bool serialize(std::ostream& os) const = 0;
    /**
     * @brief deserialize - restore object from stream
     * @param is  - byte istream
     * @return success of operation
     */
    virtual bool deserialize(std::istream& is) = 0;
    /**
     * @brief ~ISerializable virtual destructor
     */
    virtual ~ISerializable() = 0;
};
inline ISerializable::~ISerializable() { }
}
#endif
