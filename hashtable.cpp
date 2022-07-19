#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __HASHTABLE__MODULE__
#define __HASHTABLE__MODULE__


#include "constants.cpp"
#include "types.cpp"


struct PerftHashTableEntry {
  u64_t validation=0;
  int depth=0;
  u64_t count=0;
};


class PerftHashTable {
  private:
    PerftHashTableEntry table[1 << 16];
    u64_t mask = (1 << 16) - 1;

  public:
    u64_t get(u64_t hash) {
      int index = hash & this->mask;
      return table[index].count;
    };

    void set(u64_t hash, u64_t validation, int depth, u64_t count) {
      int index = hash & this->mask;
      table[index].validation = validation;
      table[index].depth = depth;
      table[index].count = count;
    };

    bool contains(u64_t hash, u64_t validation, int depth) {
      int index = hash & this->mask;
      return table[index].validation == validation && table[index].depth == depth;
    };
};


#endif // __HASHTABLE__MODULE__