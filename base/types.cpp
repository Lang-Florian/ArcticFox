#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif
#ifndef __TYPES__MODULE__
#define __TYPES__MODULE__


#define u8_t unsigned char
#define u16_t unsigned short
#define u32_t unsigned int
#define u64_t unsigned long long
#define i8_t char
#define i16_t short
#define i32_t int
#define i64_t long long


#define bitboard_t u64_t
#define castling_t u8_t
#define color_t u8_t
#define hash_t u64_t
#define move_t u32_t
#define movetype_t u8_t
#define outcome_t u8_t
#define piece_t u8_t
#define score_t i16_t
#define square_t u8_t


#define move_stack_t stack::Stack<move_t, MAX_MOVE_GENERATION_SIZE>
#define pv_t stack::Stack<move_t, MAX_PV_DEPTH>


#endif