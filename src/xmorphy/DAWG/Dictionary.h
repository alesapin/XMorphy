#ifndef DICTIONARY_H
#define DICTIONARY_H
#include "dawgdic/dictionary.h"
#include "dawgdic/dictionary-builder.h"
#include "dawgdic/dawg-builder.h"
#include "dawgdic/completer.h"
#include "dawgdic/guide.h"
#include "ISerializable.h"
#include "BuildFactory.h"
#include "Exceptions.h"
#include <type_traits>
#include <fstream>
namespace dawg {
/**
 * @brief The DictionaryBase class represent DAWG,
 * with additional data and ability to complete and search
 * keys. It should not be used for instanciation.
 */
template<typename T>
class DictionaryBase  {
    /**
     * factory and iterator are friends.
     */
    template<typename U>
    friend class BuildFactory;

    friend class ConstIterator;

protected:
    /**
     * @brief data
     */
    std::vector<T> data;
    /**
     * @brief dict from dawgdic library,
     * that can store only int values
     */
    dawgdic::Dictionary* dict;
    /**
     * @brief guide supporting data structure
     * to implement completer
     */
    dawgdic::Guide* guide;
    /**
     * @brief comp class for key complition
     */
    dawgdic::Completer* comp;
    /**
     * @brief getValue - returns value from index
     * @param index in data array
     * @return value
     */
    const T& getValue(uint index) const {
        return data[index];
    }
    /**
     * @brief setData - replaces data vector with d
     * @param d - new data
     */
    void setData(const std::vector<T>& d){
        data = d;
    }
    /**
     * @brief init sets completers dict and guide
     * should be used by factory or deserealize method
     */
    void init(){
        comp->set_dic(*dict);
        comp->set_guide(*guide);
    }
    /**
     * @brief DictionaryBase protected constructor,because
     * this class should not be instanciated.
     * Initialize dict,guide and comp. Also cheks T for limitations.
     */
    DictionaryBase() {
        static_assert(std::is_base_of<ISerializable, T>::value || std::is_pod<T>::value, "Type T must be POD or implement ISerializable!");
        dict = new dawgdic::Dictionary();
        comp = new dawgdic::Completer();
        guide = new dawgdic::Guide();
    }
    /**
     * @brief start - method, which sets completer to
     * start with given prefix. Need for iterator implementation.
     * @param prefix - prefix string, that should be completed
     * @return success of starting.
     */
    bool start(const std::string& prefix) {
      uint index = dict->root();
      if(dict->Follow(prefix.c_str(),&index)) {
        comp->Start(index, prefix.c_str());
        return true;
      }
      return false;
    }
    /**
     * @brief next connected with start method. Used for iteration.
     * @return hasNext node?
     */
    bool next() {
        return comp->Next();
    }
    /**
     * @brief getCurrentKey connected with start method.
     * @return current key on this step
     */
    std::string getCurrentKey() const {
        return std::string(comp->key());
    }
    /**
     * @brief getCurrentValue connected with start method.
     * @return current value on this step
     */
    const T& getCurrentValue() const {
        return getValue(comp->value());
    }


public:
    /**
     * @brief The DawgRecord - struct, that used
     * to implement Key-Value pair of DAWG.
     */
    struct DawgRecord{
        std::string key;
        T value;
    };
    /**
     * @brief The ConstIterator Constant iterator
     * to traversal DAWG
     */
    struct ConstIterator {
        /**
         *Friend to DAWG
         */
        friend class DictionaryBase<T>;
        /**
         * @brief Default constructor
         */
        ConstIterator(){}
        /**
         * @brief ConstIterator copy constructor.
         * @param o - object to copy
         */
        ConstIterator(const ConstIterator&o){
            at = o.at;
            parent = o.parent;
        }
        /**
         * @brief operator =
         * @param o object to copy
         * @return this
         */
        ConstIterator& operator=(const ConstIterator&o){
            parent = o.parent;
            at = o.at;
            return *this;
        }
        /**
         * @brief operator++ - increment iterator
         * @return this
         */
        ConstIterator& operator++() {
            if(parent->next()) {
                at.key = parent->getCurrentKey();
                at.value = parent->getCurrentValue();
            } else {
                at.key = '\0';
                parent = 0;
            }
            return *this;
        }
        /**
         * @brief operator == checks for equlity
         * @param o other object
         * @return bool
         */
        bool operator==(const ConstIterator& o) {
            return at.key == o.at.key && parent == o.parent;
        }
        /**
         * @brief operator!= check for non equality
         * @param o other object
         * @return bool
         */
        bool operator!=(const ConstIterator& o) {return !(*this == o);}
        /**
         * @brief operator* get iteration record
         * @return object
         */
        DawgRecord operator*() const {return at;}
        /**
         * @brief operator -> get const reference to object
         * @return const ref
         */
        const DawgRecord* operator->() const {return &at;}

    private:
        /**
         * @brief ConstIterator - constructor used by begin method
         * @param par - object, that will be iteratred
         */
        ConstIterator(DictionaryBase<T>* par):parent(par){
            if(parent){
                at.key = par->getCurrentKey();
                at.value = par ->getCurrentValue();
            }else{
                at.key = '\0';
            }
        }
        /**
         * @brief parent - dictionary object
         */
        DictionaryBase<T> *parent;
        /**
         * @brief at record
         */
        DawgRecord at;
    };
    /**
     * @brief begin - get begin iterator
     * @param prefix - prefix to start traversal
     * @return - iterator
     */
    ConstIterator begin(std::string prefix = "") {
        start(prefix);
        next();
        return ConstIterator(this);
    }
    /**
     * @brief end - get end iterator
     * @return - iterator
     */
    ConstIterator end() const {
        ConstIterator result(0);
        result.at.key = '\0';
        return result;
    }
    /**
     * @brief size of data
     * @return size
     */
    std::size_t size() const {
        return data.size();
    }
    /**
     * @brief contains - method to check contains of key in Dictionary
     * @param str - key
     * @return contains or not
     */
    bool contains(const std::string& str) const {
        return dict->Contains(str.c_str(), str.length());
    }
    /**
     * NOT EXCEPTION SAFE - check containty
     * @brief getValue - returns value of the given key.
     * @param str - key
     * @return value
     */
    const T &getValue(const std::string& str) const {
        int index = dict->Find(str.c_str(), str.length());
        if(index != -1) {
            return data[index];
        } else {
            throw NoSuchKeyException(str);
        }
    }
    /**
     * @brief getAllValues - get all values of dictionary
     * @return vector of values
     */
    std::vector<T> getAllValues() const {
        return data;
    }
    /**
     * @brief getAllKeys - get all keys of dictionary
     * @return vector of keys
     */
    std::vector<std::string> getAllKeys() const {
        start("");
        std::vector<std::string> result;
        while(next()){
            result.push_back(getCurrentKey());
        }
        return result;
    }



    /**
     * @brief completeKey returns all posible prefix completions
     * which contains in dict
     * @param prefix - string to complete
     * @return all posible matches
     */
    std::vector<std::string> completeKey(const std::string& prefix) {
        std::vector<std::string> result;
        if(start(prefix)){
            while(next()){
                result.push_back(getCurrentKey());
            }
        }
        return result;
    }
    /**
     * @brief completeKey returns all posible prefix completions values
     * which contains in dict
     * @param prefix - string to complete
     * @return all posible matches' values
     */
    std::vector<T> completeValues(const std::string& prefix) {
        std::vector<T> result;
        if(start(prefix)) {
            while(comp->Next()){
                result.push_back(getCurrentValue());
            }
        }
        return result;
    }
    DawgRecord followWord(const std::string& val){
      uint index;
      int startPos = 0, endPos = val.length();
      while(dict->Follow(val[startPos], &index) && startPos < endPos){
          startPos++;
      }
      if(dict->has_value(index)){
        return {val.substr(0,startPos), data[dict->value(index)]};
      }
      throw NoSuchKeyException(val);
    }
    /**
     * @brief clear data in dictionary
     */
    void clear(){
        dict->Clear();
        guide->Clear();
        data.clear();
    }
    /**
    * @brief ~DictionaryBase - destructor
    */
   virtual  ~DictionaryBase() {
        delete dict;
        delete guide;
        delete comp;
    }
};

/**
 * Bicycle to specialize two method for current type of T
 */
template<typename T, typename Enable = void>
class Dictionary;


/**
 * @brief The Dictionary<Serial, std::enable_if<std::is_base_of<ISerializable, Serial>::value>::type>
 * achieved by use of type_traits, only ISerializable inheritors
 */
template <typename Serial>
class Dictionary<Serial, typename std::enable_if<std::is_base_of<ISerializable, Serial>::value>::type> : public ISerializable, public DictionaryBase<Serial> {
public:
    /**
     * Make parent constructor public
     */
    using DictionaryBase<Serial>::DictionaryBase;

    /**
     * @brief serialize - save object to output stream
     * @param os - byte stream
     * @return success of operation
     */
    bool serialize(std::ostream& os) const {
        if(this->dict->Write(&os) && this->guide->Write(&os)){
            unsigned dataSize = static_cast<unsigned>(this->data.size());
            if (!os.write(reinterpret_cast<const char *>(&dataSize),
                               sizeof(unsigned))) {
                return false;
            }
            for(size_t i = 0; i < this->data.size(); ++i){
                if (!this->data[i].serialize(os)) {
                    return false;
                }
            }
        }
        return true;
    }
    /**
    * @brief deserialize - reestablish object from input stream
    * @param is - byte stream
    * @return success of operation
    */
   bool deserialize(std::istream& is) {
        if(this->dict->Read(&is) && this->guide->Read(&is)){
            unsigned dataSize;
            if (!is.read(reinterpret_cast<char *>(&dataSize), sizeof(unsigned))) {
                return false;
            }
            std::vector<Serial> newData(dataSize);
            size_t i = 0;
            while(i < dataSize) {
                 if(!newData[i].deserialize(is)) return false;
                 i++;
            }
            this->init();
            this->data.swap(newData);
        }else{
               return false;
        }
        return true;
    }

};
/**
 * @brief The Dictionary<POD, std::enable_if<std::is_pod<POD>::value>::type> class
 * achieved by use of type_traits, only POD structures
 */
template <typename POD>
class Dictionary<POD, typename std::enable_if<std::is_pod<POD>::value>::type> : public ISerializable, public DictionaryBase<POD> {

public:
    /**
     *Make parent constructor public
     */
    using DictionaryBase<POD>::DictionaryBase;
    /**
     * @brief serialize - save object to output stream
     * @param os - byte stream
     * @return success of operation
     */
    bool serialize(std::ostream& os) const {
        if(this->dict->Write(&os)  && this->guide->Write(&os)) {
            unsigned dataSize = static_cast<unsigned>(this->data.size());
            if (!os.write(reinterpret_cast<const char *>(&dataSize),
                               sizeof(unsigned))) {
                return false;
            }
            if(!os.write(reinterpret_cast<const char *>(&(this->data[0])),dataSize*sizeof(POD))){
                return false;
            }
        }
        return true;
    }
    /**
    * @brief deserialize - reestablish object from input stream
    * @param is - byte stream
    * @return success of operation
    */
    bool deserialize(std::istream& is) {
        if(this->dict->Read(&is) && this->guide->Read(&is)){
            unsigned dataSize;
            if (!is.read(reinterpret_cast<char *>(&dataSize), sizeof(unsigned))) {
                return false;
            }
            std::vector<POD> newData(dataSize);
            if (!is.read(reinterpret_cast<char *>(&newData[0]),
                          sizeof(POD) * dataSize)) {
                return false;
            }
            this->init();
            this->data.swap(newData);
        }else{
               return false;
        }
        return true;
    }
};

}
#endif
