#include <limits>
#include <vector>

#include "CacheToolbox.h"
#include "LRUCache.h"

LRUInternals::LRUInternals(const CacheConfig& config) : config(config)
{
    cache.resize(config.get_nb_sets());

    for (auto& set: cache)
    {
        set.lines.resize(config.get_associativity());
        set.max_age = 0;

        for (auto& line : set.lines)
        {
            line.tag = 0;
            line.time = TIME_INVALID;
            line.tid = 0;
        }
    }
}

LRUInternals::LRUInternals(unsigned int size, unsigned int associativity, unsigned int cache_line_size)
    : LRUInternals(CacheConfig(size, associativity, cache_line_size)) {}

SimpleLRUCache::SimpleLRUCache(const CacheConfig& config) : internals(config)
{}

SimpleLRUCache::SimpleLRUCache(unsigned int size, unsigned int associativity, unsigned int cache_line_size)
    : SimpleLRUCache(CacheConfig(size, associativity, cache_line_size)) {}

bool SimpleLRUCache::access(addr_t address, unsigned tid)
{
    //Do we need to align the address?
    const addr_t aligned_address = align_on_cache_line_size(address, internals.config.get_cache_line_size());

    const addr_t set_index = get_set_index(aligned_address, internals.config);
    const addr_t tag = get_tag(aligned_address, internals.config);

    LRUSet& set = internals.cache[set_index];

    age_t min_age = std::numeric_limits<age_t>::max();

    auto victim = set.lines.end();
    for (auto way = set.lines.begin(); way != set.lines.end(); ++way)
    {
        //Cache hit
        if (way->tag == tag && way->time != TIME_INVALID && way->tid == tid)
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
    victim->tid = tid;

    return false;
}


std::vector<LRUCacheLine>::iterator find_LRU_victim(LRUSet& set)
{
    auto victim = set.lines.end();
    age_t min_age = std::numeric_limits<age_t>::max();

    for (auto way = set.lines.begin(); way < set.lines.end(); ++way)
    {
        if (way->time < min_age)
        {
            victim = way;
            min_age = victim->time;
        }
    }
    return victim;
}
