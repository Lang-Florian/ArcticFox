#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __TYPES__MODULE__
#define __TYPES__MODULE__


/*

  most used types

*/


#define u8_t unsigned char
#define u16_t unsigned short
#define u32_t unsigned int
#define u64_t unsigned long long
#define s8_t char
#define s16_t short
#define s32_t int
#define s64_t long long


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


#endif // __TYPES__MODULE__