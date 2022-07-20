#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __HASHTABLE__MODULE__
#define __HASHTABLE__MODULE__


#include "constants.cpp"
#include "types.cpp"


struct HashTableEntry {
  hash_t hash;
};


class HashTable {
  private:
    HashTableEntry table[HASH_TABLE_SIZE];
    hash_t mask = HASH_TABLE_SIZE - 1;

  public:

};


#endif // __HASHTABLE__MODULE__