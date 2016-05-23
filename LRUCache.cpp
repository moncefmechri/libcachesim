#include <limits>
#include "CacheToolbox.h"

#include "LRUCache.h"


LRUCache::LRUCache(const CacheConfig& config) : config(config)
{
    cache.resize(config.get_nb_sets());

    for (auto& set: cache)
    {
        set.lines.resize(config.get_associativity());//TODO: init the lines
        set.max_age = 0;
    }
}

LRUCache::LRUCache(unsigned int size, unsigned int associativity, unsigned int cache_line_size)
    : LRUCache(CacheConfig(size, associativity, cache_line_size)) {}

bool LRUCache::access(addr_t address)
{
    //Do we need to align the address?
    const addr_t aligned_address = align_on_cache_line_size(address, config.get_cache_line_size());

    const addr_t set_index = get_set_index(aligned_address, config);
    const addr_t tag = get_tag(aligned_address, config);

    LRUSet& set = cache[set_index];

    unsigned min_age = std::numeric_limits<unsigned>::max();

    auto victim = set.lines.end();
    for (auto way = set.lines.begin(); way != set.lines.end(); ++way)
    {
        //Cache hit
        if (way->tag == tag && way->time != TIME_INVALID)
        {
            way->time = ++set.max_age;
            return true;
        }

        else
        {
            if (way->time < min_age)
            {
                min_age = way->time;
                victim = way;
            }
        }
    }
    //If we reach this point, there it's a cache miss

    victim->tag = tag;
    victim->time = ++set.max_age;

    return false;
}
