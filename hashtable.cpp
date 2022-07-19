#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __HASHTABLE__MODULE__
#define __HASHTABLE__MODULE__


#include "constants.cpp"
#include "types.cpp"


struct PerftHashTableEntry {
  hash_t hash;
  int depth;
  u64_t count;
};


class PerftHashTable {
  private:
    PerftHashTableEntry table[1 << 16];
    hash_t mask = (1 << 16) - 1;

  public:
    u64_t get(hash_t hash) {
      int index = hash & this->mask;
      return table[index].count;
    };

    void set(hash_t hash, int depth, u64_t count) {
      int index = hash & this->mask;
      table[index].hash = hash;
      table[index].depth = depth;
      table[index].count = count;
    };

    bool contains(hash_t hash, int depth) {
      int index = hash & this->mask;
      return table[index].hash == hash && table[index].depth == depth;
    };
};


#endif // __HASHTABLE__MODULE__