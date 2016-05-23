#pragma once

#include <vector>

#include "CacheToolbox.h"

#define TIME_INVALID 0

struct LRUCacheLine
{
    addr_t tag;
    unsigned time;
};

struct LRUSet
{
    std::vector<LRUCacheLine> lines;
    unsigned max_age;
};

class LRUCache
{
    CacheConfig config;
    std::vector<LRUSet> cache;

public:

    LRUCache(const CacheConfig& config);
    LRUCache(unsigned size, unsigned associativity, unsigned cache_line_size);

    bool access(addr_t address);
};
