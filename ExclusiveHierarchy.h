#pragma once

#include <vector>

#include "CacheToolbox.h"
#include "LRUCache.h"

class ExclusiveHierarchy
{
    CacheConfig L1_config;
    CacheConfig L2_config;

    std::vector<LRUSet> L1_cache;
    std::vector<LRUSet> L2_cache;

    std::vector<LRUCacheLine>::iterator find_victim(LRUSet& set) const;
public:
    enum class ACCESS_STATUS {L1_HIT, L1_MISS_L2_HIT, L1_MISS_L2_MISS};

    ExclusiveHierarchy(const CacheConfig& L1_config, const CacheConfig& L2_config);
    
    ACCESS_STATUS access(addr_t address);
};
