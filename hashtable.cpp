#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __HASHTABLE__MODULE__
#define __HASHTABLE__MODULE__


#include <array>

#include "constants.cpp"
#include "types.cpp"


struct HashTableEntry {
  hash_t hash;
  eval_t eval;
  move_t move;
  u8_t depth;
  u8_t flags;
};


template <int size>
class HashTable {
  private:
    std::array<HashTableEntry, size> table;
    hash_t mask = size - 1;

  public:    
    HashTableEntry& get(hash_t hash) {
      return table[hash & mask];
    };
};


constexpr u8_t exact_bound = 0;
constexpr u8_t lower_bound = 1;
constexpr u8_t upper_bound = 2;


#endif // __HASHTABLE__MODULE__