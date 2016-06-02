#include <stdexcept>
#include <limits>

#include "ExclusiveHierarchy.h"
#include "LRUCache.h"

ExclusiveHierarchy::ExclusiveHierarchy(const CacheConfig& L1_config, const CacheConfig& L2_config) : L1_config(L1_config),  L2_config(L2_config)
{
    if (L1_config.get_cache_line_size() != L2_config.get_cache_line_size())
        throw std::runtime_error("The caches in the exclusive hierarchy don't have the same cache line size");

    L1_cache.resize(L1_config.get_nb_sets());

    for (auto& set : L1_cache)
    {
        set.lines.resize(L1_config.get_associativity());
        set.max_age = 0;
    }

    L2_cache.resize(L2_config.get_nb_sets());

    for (auto & set : L2_cache)
    {
        set.lines.resize(L2_config.get_associativity());
        set.max_age = 0;
    }
}

ExclusiveHierarchy::ACCESS_STATUS ExclusiveHierarchy::access(addr_t address)
{
    /* WARNING: For now, only the entire address is stored (in the way->tag field), which means the
    * tag needs to be recomputed for each way, for each L2 access. This can be optimized
    * by storing both the address and tag in separate fields */

    //Do we need to align the address?
    const addr_t aligned_address = align_on_cache_line_size(address, L1_config.get_cache_line_size());

    const addr_t L1_set_index = get_set_index(aligned_address, L1_config);
    const addr_t L1_tag = get_tag(aligned_address, L1_config);

    LRUSet& L1_set = L1_cache[L1_set_index];

    unsigned L1_min_age = std::numeric_limits<unsigned>::max();

    auto L1_victim = L1_set.lines.end();

    for (auto way = L1_set.lines.begin(); way < L1_set.lines.end(); ++way)
    {
        //L1 hit
        if (get_tag(way->tag, L1_config) == L1_tag && way->time != TIME_INVALID)
        {
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

    unsigned L2_min_age = std::numeric_limits<unsigned>::max();

    auto L2_victim = L2_set.lines.end();

    for (auto way = L2_set.lines.begin(); way < L2_set.lines.end(); ++way)
    {

        //L2 hit
        if (way->tag == L2_tag && way->time != TIME_INVALID)
        {
            //The L1 victim is valid and must therefore be installed in L2
            if (L1_min_age != TIME_INVALID)
            {
                /* Compute the L2 tag from the full address stored in the L1 victim's tag field.
                 * See the comment at the beginning of this method */
                way->tag = get_tag(L1_victim->tag, L2_config);
                way->time = ++L2_set.max_age;
            }

            //The L1 victim is invalid. We don't need to put it in L2, so simply invalidate the L2 hit entry
            else
                way->time = TIME_INVALID;

            //Install the new line in L1
            L1_victim->tag = aligned_address;
            L1_victim->time = ++L1_set.max_age;

            return ACCESS_STATUS::L1_MISS_L2_HIT;
        }

        else
        {
            if (way->time < L2_min_age)
            {
                L2_min_age = way->time;
                L2_victim = way;
            }
        }
    }
    //L2 miss

    //Install the L1 victim in L2 only if it is valid
    if (L1_min_age != TIME_INVALID)
    {
        /* Compute the L2 tag from the full address stored in the L1 victim's tag field.
         * See the comment at the beginning of this method */
        L2_victim->tag = get_tag(L1_victim->tag, L2_config);
        L2_victim->time = ++L2_set.max_age;
    }

    //Install the new line in L1
    L1_victim->tag = aligned_address;
    L1_victim->time = ++L1_set.max_age;

    return ACCESS_STATUS::L1_MISS_L2_MISS;
}
