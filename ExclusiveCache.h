#pragma once

#include <vector>

#include "CacheToolbox.h"
#include "LRUCache.h"

class ExclusiveCache
{
    CacheConfig L2_config;
    std::vector<LRUSet> L2_cache;

public:
    enum class ACCESS_STATUS {L1_HIT, L1_MISS_L2_HIT, L1_MISS_L2_MISS};

    ExclusiveCache(const CacheConfig& L2_config);

    ACCESS_STATUS access(addr_t address, LRUCache& L1_cache, unsigned tid);
};

