#pragma once

#include <bitset>
#include <iostream>
#include <cstdint>
#include <exception>
#include <cmath>

typedef uint64_t addr_t;
typedef uint64_t age_t;

template <typename T>
bool is_power_of_2(const T& value)
{
    return (value != 0) && ((value & (value - 1)) == 0);
}

template <typename T>
T extract_bits(const T& value, size_t begin, size_t nb_bits)
{
    T mask = 0;//Initialize just to silent compiler warnings
    mask = ~(mask & 0); //trick to set all the bits of mask to 1 regardless of T's size

    return (value >> begin) & (mask >> (sizeof(T) * 8 - nb_bits));
}

template <typename T>
T align_on_cache_line_size(const T& addr, size_t cache_line_size)
{
    return addr & (T) ~(cache_line_size - 1);
}

template <typename T>
void print_binary(const T& value)
{
    std::cout << std::bitset<sizeof(T) * 8>(value) << std::endl;
}

class CacheConfig
{
private:
    //User-defined
    unsigned size; //in bytes
    unsigned assoc;
    unsigned cache_line_size;

    //Computed
    unsigned nb_sets;
    unsigned set_index_size; //Number of bits used for the index
    unsigned set_index_shift;
    unsigned tag_size; //Number of bits used for the tags
    unsigned tag_shift;

public:
    CacheConfig(unsigned size, unsigned associativity, unsigned cache_line_size);

    unsigned get_size(void) const;
    unsigned get_associativity(void) const;
    unsigned get_cache_line_size(void) const;
    unsigned get_nb_sets(void) const;
    unsigned get_set_index_size(void) const;
    unsigned get_set_index_shift(void) const;
    unsigned get_tag_size(void) const;
    unsigned get_tag_shift(void) const;
};

addr_t get_set_index(addr_t addr, unsigned index_shift, size_t index_size);
addr_t get_set_index(addr_t addr, const CacheConfig& config);
addr_t get_tag(addr_t addr, unsigned tag_shift, size_t tag_size);
addr_t get_tag(addr_t addr, const CacheConfig& config);
