#pragma once

#include <vector>

#include "CacheToolbox.h"
#include "ExclusiveCache.h"
#include "LRUCache.h"

class SimpleExclusiveHierarchy
{
    LRUCache L1_cache;
    ExclusiveCache exclusive_cache;
public:
    SimpleExclusiveHierarchy(const CacheConfig& L1_config, const CacheConfig& L2_config);
    ExclusiveCache::ACCESS_STATUS access(addr_t address);
};
