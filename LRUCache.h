#pragma once

#include <vector>

#include "CacheToolbox.h"

#define TIME_INVALID 0

struct LRUCacheLine
{
    addr_t tag;
    age_t time;
};

struct LRUSet
{
    std::vector<LRUCacheLine> lines;
    age_t max_age;
};

struct LRUInternals
{
    CacheConfig config;
    std::vector<LRUSet> cache;

    LRUInternals(const CacheConfig& config);
    LRUInternals(unsigned size, unsigned associativity, unsigned cache_line_size);
};

class SimpleLRUCache
{
    LRUInternals internals;
public:

    SimpleLRUCache(const CacheConfig& config);
    SimpleLRUCache(unsigned size, unsigned associativity, unsigned cache_line_size);

    bool access(addr_t address);
};

//Given an LRU set, return the victim that will be chosen next for eviction
std::vector<LRUCacheLine>::iterator find_LRU_victim(LRUSet& set);
