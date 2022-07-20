#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __TYPES__MODULE__
#define __TYPES__MODULE__


#include <algorithm>
#include <iostream>
#include <string>


/*

  most used types

*/


#define u8_t unsigned char
#define u16_t unsigned short
#define u32_t unsigned int
#define u64_t unsigned long long
#define u128_t unsigned __int128
#define i8_t char
#define i16_t short
#define i32_t int
#define i64_t long long
#define i128_t __int128


#define bitboard_t u64_t
#define castling_t u8_t
#define color_t u8_t
#define eval_t int
#define gen_t u8_t
#define hash_t u64_t
#define move_t u32_t
#define move_stack_t Stack<move_t, MAX_MOVE_LENGTH>
#define outcome_t u8_t
#define piece_t u8_t
#define square_t u8_t


std::ostream& operator<<(std::ostream &out, i128_t x) {
  std::string string;
  while (x > 0) {
    string.push_back(x % 10 + '0');
    x /= 10;
  };
  std::reverse(string.begin(), string.end());
  return out << string;
};

std::ostream& operator<<(std::ostream &out, u128_t x) {
  std::string string;
  while (x > 0) {
    string.push_back(x % 10 + '0');
    x /= 10;
  };
  std::reverse(string.begin(), string.end());
  return out << string;
};


#endif // __TYPES__MODULE__