#include <limits>

#include "ExclusiveCache.h"
#include "LRUCache.h"

ExclusiveCache::ExclusiveCache(const CacheConfig& L2_config) : L2_config(L2_config)
{
    L2_cache.resize(L2_config.get_nb_sets());

    for (auto & set : L2_cache)
    {
        set.lines.resize(L2_config.get_associativity());
        set.max_age = 0;

        for (auto& line : set.lines)
        {
            line.tag = 0;
            line.time = TIME_INVALID;
            line.tid = 0;
        }
    }
}

ExclusiveCache::ACCESS_STATUS ExclusiveCache::access(addr_t address, LRUInternals& L1_cache_internals, unsigned tid)
{
    /* WARNING: For now, only the entire address is stored (in the way->tag field), which means the
    * tag needs to be recomputed for each way, for each L2 access. This can be optimized
    * by storing both the address and tag in separate fields */

    //Do we need to align the address?
    const addr_t aligned_address = align_on_cache_line_size(address, L1_cache_internals.config.get_cache_line_size());

    const addr_t L1_set_index = get_set_index(aligned_address, L1_cache_internals.config);
    const addr_t L1_tag = get_tag(aligned_address, L1_cache_internals.config);

    LRUSet& L1_set = L1_cache_internals.cache[L1_set_index];

    age_t L1_min_age = std::numeric_limits<age_t>::max();

    auto L1_victim = L1_set.lines.end();

    for (auto way = L1_set.lines.begin(); way < L1_set.lines.end(); ++way)
    {
        if (get_tag(way->tag, L1_cache_internals.config) == L1_tag && way->time != TIME_INVALID && way->tid == tid)
        {//L1 hit
            way->time = ++L1_set.max_age;
            return ACCESS_STATUS::L1_HIT;
        }

        else
        {
            if (way->time < L1_min_age)
            {
                L1_min_age = way->time;
                L1_victim = way;
            }
        }
    }

    //At this point, we have an L1 miss. We need to check L2
    const addr_t L2_set_index = get_set_index(aligned_address, L2_config);
    const addr_t L2_tag = get_tag(aligned_address, L2_config);

    LRUSet& L2_set = L2_cache[L2_set_index];

    bool L2_hit = false;
    for (auto way = L2_set.lines.begin(); way < L2_set.lines.end() && !L2_hit; ++way)
    {
        //L2 hit
        if (way->tag == L2_tag && way->time != TIME_INVALID && way->tid == tid)
        {
            /* invalidate immediately the L2 entry. Important to do it first, so that this entry
             * can potentially be reused immediately (this happen only if the L1 victim belongs to this set,
             * and if this slot is the first invalid slot) */
            way->time = TIME_INVALID;

            L2_hit = true;
        }
    }

    /* The L1 victim is valid and must be installed in L2.
     * Important: The set where the L1 victim will be installed is not necessarily the same as
     * the set where the requested line was searched!*/
    if (L1_min_age != TIME_INVALID)
    {
        const addr_t L2_victim_set_index = get_set_index(L1_victim->tag, L2_config);
        LRUSet& L2_victim_set = L2_cache[L2_victim_set_index];
        auto L2_victim = find_LRU_victim(L2_victim_set);
        L2_victim->tag = get_tag(L1_victim->tag, L2_config);
        L2_victim->time = ++L2_victim_set.max_age;
        L2_victim->tid = L1_victim->tid;
    }

    //Install the new line in L1
    L1_victim->tag = aligned_address;
    L1_victim->time = ++L1_set.max_age;
    L1_victim->tid = tid;

    return L2_hit ? ACCESS_STATUS::L1_MISS_L2_HIT : ACCESS_STATUS::L1_MISS_L2_MISS;
}
