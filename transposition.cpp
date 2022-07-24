#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __TRANSPOSITION__MODULE__
#define __TRANSPOSITION__MODULE__


#include <array>

#include "constants.cpp"
#include "types.cpp"


namespace transposition {
  constexpr u64_t index_mask = (1ULL << TABLE_SIZE_LOG2) - 1;

  constexpr u64_t validation_mask = 0xFFFFFFFF00000000;
  constexpr u16_t validation_shift = 32;

  constexpr u32_t depth_mask = 0x000000FF;
  constexpr u32_t depth_shift = 0;

  constexpr u32_t eval_mask = 0x00FFFF00;
  constexpr u32_t eval_shift = 8;

  constexpr u32_t bound_mask = 0xFF000000;
  constexpr u32_t bound_shift = 24;

  constexpr u8_t exact_bound = 0;
  constexpr u8_t lower_bound = 1;
  constexpr u8_t upper_bound = 2;


  struct entry_t {
    u32_t hash_validation;
    move_t move;
    u32_t flags;

    void set(hash_t hash, move_t move, eval_t eval, u8_t depth, u8_t bound) {
      this->hash_validation = (hash & validation_mask) >> validation_shift;
      this->move = move;
      this->flags = (
        (depth << depth_shift) |
        (eval << eval_shift) |
        (bound << bound_shift)
      );
    };

    move_t get_move() {
      return this->move;
    };

    u8_t get_depth() {
      return (flags & depth_mask) >> depth_shift;
    };

    eval_t get_eval() {
      return (flags & eval_mask) >> eval_shift;
    };

    u8_t get_bound() {
      return (flags & bound_mask) >> bound_shift;
    };

    bool is_valid(hash_t hash, u8_t depth) {
      return (
        ((hash & validation_mask) >> validation_shift == this->hash_validation) &&
        (((flags & depth_mask) >> depth_shift) >= depth)
      );
    };
  };


  static entry_t table[1ULL << TABLE_SIZE_LOG2];


  entry_t& get(hash_t hash) {
    return table[hash & index_mask];
  };


  u64_t table_size() {
    return sizeof(entry_t) * (1ULL << TABLE_SIZE_LOG2);
  };
};


#endif // __TRANSPOSITION__MODULE__