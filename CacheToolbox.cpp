#include <stdexcept>
#include "CacheToolbox.h"

CacheConfig::CacheConfig(unsigned size, unsigned associativity, unsigned cache_line_size) : size(size), assoc(associativity), cache_line_size(cache_line_size)
{
    /* For simplicity, we currently enforce that the cache size, associativity and cache line size
    * must be power of 2 */
    if (!is_power_of_2(size))
        throw std::runtime_error("Cache size must be a power of 2");

    if (!is_power_of_2(associativity))
        throw std::runtime_error("Associativity must be a power of 2");

    if (!is_power_of_2(cache_line_size))
        throw std::runtime_error("Cache line size must be a power of 2");

    nb_sets = size / cache_line_size / associativity;

    const unsigned log2_line_size = std::log2(cache_line_size);

    set_index_size = std::log2(nb_sets);
    set_index_shift = log2_line_size;

    tag_size = sizeof(addr_t) * 8 - log2_line_size - set_index_size;
    tag_shift = log2_line_size + set_index_size;
}

unsigned CacheConfig::get_size(void) const
{
    return size;
}

unsigned CacheConfig::get_associativity(void) const
{
    return assoc;
}

unsigned CacheConfig::get_cache_line_size(void) const
{
    return cache_line_size;
}

unsigned CacheConfig::get_nb_sets(void) const
{
    return nb_sets;
}

unsigned CacheConfig::get_set_index_size(void) const
{
    return set_index_size;
}

unsigned CacheConfig::get_set_index_shift(void) const
{
    return set_index_shift;
}

unsigned CacheConfig::get_tag_size(void) const
{
    return tag_size;
}

unsigned CacheConfig::get_tag_shift(void) const
{
    return tag_shift;
}

addr_t get_set_index(addr_t addr, unsigned index_shift, size_t index_size)
{
    return extract_bits(addr, index_shift, index_size);
}

addr_t get_set_index(addr_t addr, const CacheConfig& config)
{
    return get_set_index(addr, config.get_set_index_shift(), config.get_set_index_size());
}

addr_t get_tag(addr_t addr, unsigned tag_shift, size_t tag_size)
{
    return extract_bits(addr, tag_shift, tag_size);
}

addr_t get_tag(addr_t addr, const CacheConfig& config)
{
    return get_tag(addr, config.get_tag_shift(), config.get_tag_size());
}

CacheStats::CacheStats(void)
{
    accesses = 0;
    misses = 0;
}

void CacheStats::log_access(void)
{
    ++accesses;
}

void CacheStats::log_miss(void)
{
    ++misses;
}

uint64_t CacheStats::get_accesses(void)
{
    return accesses;
}

uint64_t CacheStats::get_misses(void)
{
    return misses;
}
