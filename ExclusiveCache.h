#pragma once

#include <vector>

#include "CacheToolbox.h"
#include "LRUCache.h"

/* This class represents the L2 cache of an exclusive cache hierarchy and is meant as a building block
 * to build exclusive cache hierarchies. See the class SimpleExclusiveHierarchy for an example */
class ExclusiveCache
{
    CacheConfig L2_config;
    std::vector<LRUSet> L2_cache;

public:
    enum class ACCESS_STATUS {L1_HIT, L1_MISS_L2_HIT, L1_MISS_L2_MISS};

    ExclusiveCache(const CacheConfig& L2_config);

    ACCESS_STATUS access(addr_t address, LRUInternals& L1_cache_internals, unsigned tid);
};

