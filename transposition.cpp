#pragma once

#include <iostream>
#include "modules/system.cpp"
#include "base.cpp"

constexpr u8_t exact_bound = 0;
constexpr u8_t lower_bound = 1;
constexpr u8_t upper_bound = 2;

constexpr u64_t _validation_mask = 0xFFFFFFFF00000000;
constexpr u16_t _validation_shift = 32;
constexpr u32_t _depth_mask = 0x000000FF;
constexpr u32_t _depth_shift = 0;
constexpr u32_t _score_mask = 0x00FFFF00;
constexpr u32_t _score_shift = 8;
constexpr u32_t _bound_mask = 0xFF000000;
constexpr u32_t _bound_shift = 24;

struct entry_t {
  u32_t hash_validation;
  move_t move;
  u32_t flags;

  void set(hash_t hash, move_t move, score_t score, u8_t depth, u8_t bound) {
    this->hash_validation = (hash & _validation_mask) >> _validation_shift;
    this->move = move;
    this->flags = (
      (depth << _depth_shift) |
      (score << _score_shift) |
      (bound << _bound_shift)
    );
  };

  move_t get_move() {
    return this->move;
  };

  u8_t get_depth() {
    return (flags & _depth_mask) >> _depth_shift;
  };

  score_t get_score() {
    return (flags & _score_mask) >> _score_shift;
  };

  u8_t get_bound() {
    return (flags & _bound_mask) >> _bound_shift;
  };

  bool is_valid(hash_t hash, u8_t depth) {
    return (
      ((hash & _validation_mask) >> _validation_shift == this->hash_validation) &&
      (((flags & _depth_mask) >> _depth_shift) >= depth)
    );
  };
};

u64_t _transposition_table_size = 1ULL << (63 - __builtin_clzll(3 * free_memory() / sizeof(entry_t) / 4));

const u64_t _index_mask = _transposition_table_size - 1;

entry_t *table = new entry_t[_transposition_table_size];

entry_t& get(hash_t hash) {
  return table[hash & _index_mask];
};

u64_t table_size() {
  return sizeof(entry_t) * _transposition_table_size;
};