#include <stdexcept>

#include "ExclusiveHierarchy.h"

#include "ExclusiveCache.h"
#include "LRUCache.h"



SimpleExclusiveHierarchy::SimpleExclusiveHierarchy(const CacheConfig& L1_config, const CacheConfig& L2_config)
    : L1_cache(L1_config), exclusive_cache(L2_config)
{
    if (L1_config.get_cache_line_size() != L2_config.get_cache_line_size())
        throw std::runtime_error("The caches in the exclusive hierarchy don't have the same cache line size");
}

ExclusiveCache::ACCESS_STATUS SimpleExclusiveHierarchy::access(addr_t address)
{
    return exclusive_cache.access(address, L1_cache, 0);
}
