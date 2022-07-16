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

#define bitboard_t u64_t
#define castling_t u8_t
#define color_t u8_t
#define gen_t u8_t
#define hash_t u64_t
#define move_t u32_t
#define outcome_t u8_t
#define piece_t u8_t
#define square_t u8_t


#endif // __TYPES__MODULE__