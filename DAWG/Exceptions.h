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
       : std::runtime_error( "String key was not found" ), str(s){}
    /**
     * @brief what - standart method
     * @return error text
     */
    virtual const char* what() const throw() {
        std::string result(std::runtime_error::what());
        result+= ":" + str;
        return result.c_str();
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
