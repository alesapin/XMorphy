#pragma once
#include <list>
#include <unordered_map>
#include <exception>
#include <stdexcept>
#include <utility>

namespace X
{

template<class Key, class Value, class KeyHash = std::hash<Key>>
class LRUCache
{
public:
    using KeyValuePair = std::pair<Key, Value>;
	using ListIterator = typename std::list<KeyValuePair>::iterator;

    LRUCache(size_t max_size_)
        : max_size(max_size_)
    {}

    void put(const Key & key, const Value & value)
    {
        auto it = cache_map.find(key);
        cache_list.push_front(KeyValuePair(key, value));
        if (it != cache_map.end())
        {
            cache_list.erase(it->second);
            cache_map.erase(it);
        }
        cache_map[key] = cache_list.begin();

		if (cache_map.size() > max_size)
        {
			auto last = cache_list.end();
			last--;
			cache_map.erase(last->first);
			cache_list.pop_back();
		}
    }
    const Value & get(const Key & key)
    {
        auto it = cache_map.find(key);
        if (it == cache_map.end())
            throw std::range_error("There is no such key in cache");

        cache_list.splice(cache_list.begin(), cache_list, it->second);
        return it->second->second;
    }

    bool exists(const Key & key)
    {
        return cache_map.count(key) != 0;
    }

	size_t size() const {
		return cache_map.size();
	}

private:
    size_t max_size;
    std::list<KeyValuePair> cache_list;
	std::unordered_map<Key, ListIterator, KeyHash> cache_map;
};

}
