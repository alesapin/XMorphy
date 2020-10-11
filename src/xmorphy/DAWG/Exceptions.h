#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>
#include <stdexcept>
#include <string>
namespace dawg{
/**
 * @brief The NoSuchKeyException - designed to
 *  throw when Dictionary doesn't contains key
 */
class NoSuchKeyException: public std::runtime_error {
public:
    /**
     * @brief NoSuchKeyException constructor
     * @param s - string that doesn't contains in Dictionary
     */
    NoSuchKeyException(const std::string& s )
       : std::runtime_error( "String key was not found" ), str(std::string(std::runtime_error::what()) + ":" + s){}
    /**
     * @brief what - standart method
     * @return error text
     */
    virtual const char* what() const throw() {
        return str.c_str();
    }
    /**
     * @brief getStr
     * @return get string key
     */
    std::string getStr() const {
        return str;
    }
private:
    /**
        * @brief str - key itself
        */
    std::string str;
};

}
#endif
