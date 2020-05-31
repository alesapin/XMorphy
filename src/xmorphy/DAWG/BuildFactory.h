#ifndef BUILD_FACTORY_H
#define BUILD_FACTORY_H

#include "dawgdic/dawg-builder.h"
#include "dawgdic/guide-builder.h"
#include "ISerializable.h"
#include "Dictionary.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <random>
namespace dawg {

template<typename T>
class BasicDawg;

template<class T,class U>
class Dictionary;

/**
 * @brief The BuildFactory - template factory for building
 * dictionaries. It is usefull, when you constract Dictionary for
 * the first time - from values.
 * There are some limitations on T, achieved by using
 *  C++11 <type_traits>. T must be POD - class or must
 * implement ISerializable interface.
 */
template<typename T>
class BuildFactory : public dawgdic::DawgBuilder {

private:
    /**
     * @brief dawg from dawgdic library representing connections
     * in graph;
     */
    dawgdic::Dawg dawg;
    /**
     * @brief data that will be stored in Dictionary
     */
    std::vector<T> data;
    /**
     * @brief cache size for insert or link
     */
    size_t CACHE_SIZE = 20;
    /**
     * @brief local cache for faster link
     */
    std::unordered_map<T, std::size_t> cache;

    void removeRandFromChache() {
        auto iter = cache.begin();
        std::advance(iter, rand()%cache.size());
        cache.erase(iter);
    }

public:
    /**
     * @brief BuildFactory - Default constructor, checks T's limitations using static_assert
     */
    BuildFactory():dawgdic::DawgBuilder(){
        static_assert(std::is_base_of<ISerializable, T>::value || std::is_pod<T>::value, "Type T must be POD or derived from ISerializable!");
    }
    /**
     * @brief insert key-value pair to factory. You should insert keys
     * is alphabetical order only.
     * @param str - key
     * @param value - value
     * @return success of operations.
     */
    bool insert(std::string str, const T& value){
        int index = data.size();
        if(dawgdic::DawgBuilder::Insert(str.c_str(),str.length(),index)) {
            if (cache.size() == CACHE_SIZE) {
                removeRandFromChache();
            }
            cache[value] = index;
            data.push_back(value);
            return true;
        }
        return false;
    }
    /**
     * @brief insert of link new key value in factory.
     * @param str - key
     * @param value - value
     * @return success of operation
     */
    bool insertOrLink(std::string str, const T& value) {
        auto iter = cache.find(value);
        int index;
        if (iter != cache.end()) {
            index = iter->second;
        } else {
            auto targ = std::find(data.begin(), data.end(), value);
            if (targ != data.end()) {
                index = targ - data.begin();
                if (cache.size() == CACHE_SIZE) {
                    removeRandFromChache();
                }
                cache[value]=index;
            } else {
                index = -1;
            }
        }
        if (index != -1) {
            return addLink(str, index);
        } else {
            return insert(str, value);
        }
    }

    /**
     * @brief adding link to existing value of dict.
     * @param key - key that should be linked
     * @param - link num
     * @return success of operation
     */
    bool addLink(std::string str, std::size_t ind){
      if(ind > data.size()) return false;
      return dawgdic::DawgBuilder::Insert(str.c_str(),str.length(),ind);
    }

    /**
     * @brief build - build Dictionary from inserted values.
     * After building, all preveosly inserted data will be cleared.
     * You should fill factory again.
     * @return shared_pointer to created dictionary.
     * Don't care abour second template parameter arg,
     * it's type_traits issue.
     */
    std::shared_ptr<Dictionary<T,void>> build(){
        DawgBuilder::Finish(&dawg);
        Dictionary<T,void>  *result = new Dictionary<T,void>();
        if(dawgdic::DictionaryBuilder::Build(dawg,result->dict)) {
            if(dawgdic::GuideBuilder::Build(dawg,*result->dict,result->guide)) {
                result->setData(data);
                result->init();
                dawg.Clear();
                data.clear();
                return std::shared_ptr<Dictionary<T,void>>(result);
            }
        }
        delete result;
        return nullptr;
    }
};
}
#endif
