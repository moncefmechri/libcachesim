#pragma once

#include <vector>

#include "CacheToolbox.h"
#include "ExclusiveCache.h"
#include "LRUCache.h"

/* An implementation of a 2-levels exclusive cache hierarchy*/
class SimpleExclusiveHierarchy
{
    LRUInternals L1_cache;
    ExclusiveCache exclusive_cache;
public:
    SimpleExclusiveHierarchy(const CacheConfig& L1_config, const CacheConfig& L2_config);
    ExclusiveCache::ACCESS_STATUS access(addr_t address);
};
