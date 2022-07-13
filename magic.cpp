#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#ifndef __MAGIC__MODULE__
#define __MAGIC__MODULE__


#include <array>

#include "constants.cpp"


/*
 
	generate the magic table at compile time
 
*/
namespace magic {
	// generate the incomplete attack rays
  namespace incomplete_attack_ray {
    constexpr std::array<bitboard_t, 64> bishop = {[]() constexpr {
      std::array<bitboard_t, 64> bishop{0ULL};
      for (square_t square = 0; square < 64; square++) {
        bishop[square] = 0ULL;
        for (int i = 1; i < square % 8 && i < square / 8; i++)
          bishop[square] |= (bitboard(square) >> (9 * i));
        for (int i = 1; i < square % 8 && i < 7 - (square / 8); i++)
          bishop[square] |= (bitboard(square) << (7 * i));
        for (int i = 1; i < 7 - (square % 8) && i < 7 - (square / 8); i++)
          bishop[square] |= (bitboard(square) << (9 * i));
        for (int i = 1; i < 7 - (square % 8) && i < square / 8; i++)
          bishop[square] |= (bitboard(square) >> (7 * i));
      };
      return bishop;
    }()};

    constexpr std::array<bitboard_t, 64> rook = {[]() constexpr {
      std::array<bitboard_t, 64> rook{0ULL};
      for (square_t square = 0; square < 64; square++) {
        rook[square] = 0ULL;
        for (int i = 1; i < square % 8; i++)
          rook[square] |= (bitboard(square) >> i);
        for (int i = 1; i < 7 - (square % 8); i++)
          rook[square] |= (bitboard(square) << i);
        for (int i = 1; i < square / 8; i++)
          rook[square] |= (bitboard(square) >> (8 * i));
        for (int i = 1; i < 7 - (square / 8); i++)
          rook[square] |= (bitboard(square) << (8 * i));
      };
      return rook;
    }()};
  };

	// generate correct bishop attacks for each square and occupancy
	namespace attack::slow {
		constexpr bitboard_t bishop(square_t square, bitboard_t occupancy) {
			bitboard_t bishop = 0ULL;
			for (int i = 1; i <= square % 8 && i <= square / 8; i++) {
				bishop |= (bitboard(square) >> (9 * i));
				if ((bitboard(square) >> (9 * i)) & occupancy) break;
			};
			for (int i = 1; i <= square % 8 && i <= 7 - (square / 8); i++) {
				bishop |= (bitboard(square) << (7 * i));
				if ((bitboard(square) << (7 * i)) & occupancy) break;
			};
			for (int i = 1; i <= 7 - (square % 8) && i <= 7 - (square / 8); i++) {
				bishop |= (bitboard(square) << (9 * i));
				if ((bitboard(square) << (9 * i)) & occupancy) break;
			};
			for (int i = 1; i <= 7 - (square % 8) && i <= square / 8; i++) {
				bishop |= (bitboard(square) >> (7 * i));
				if ((bitboard(square) >> (7 * i)) & occupancy) break;
			};
			return bishop;
		};

		constexpr bitboard_t rook(square_t square, bitboard_t occupancy) {
			bitboard_t rook = 0ULL;
			for (int i = 1; i <= square % 8; i++) {
				rook |= (bitboard(square) >> i);
				if ((bitboard(square) >> i) & occupancy) break;
			};
			for (int i = 1; i <= 7 - (square % 8); i++) {
				rook |= (bitboard(square) << i);
				if ((bitboard(square) << i) & occupancy) break;
			};
			for (int i = 1; i <= square / 8; i++) {
				rook |= (bitboard(square) >> (8 * i));
				if ((bitboard(square) >> (8 * i)) & occupancy) break;
			};
			for (int i = 1; i <= 7 - (square / 8); i++) {
				rook |= (bitboard(square) << (8 * i));
				if ((bitboard(square) << (8 * i)) & occupancy) break;
			};
			return rook;
		};
	};

	// generates possible occupancies on attack rays
	constexpr bitboard_t generate_occupancy(int index, bitboard_t attack_ray) {
		bitboard_t occupancy = bitboard::none;
		int attack_ray_size = popcount(attack_ray);
		for (int i = 0; i < attack_ray_size; i++) {
			square_t square = get_lsb(attack_ray);
			clear_bit(attack_ray, square);
			if (index & (1 << i)) occupancy |= bitboard(square);
		};
		return occupancy;
	};

	// define the magic struct
	struct magic_t {
		u64_t magic_number;
		bitboard_t mask;
		int offset;
	};

	// define the magics of the bishop
	constexpr magic_t bishop[64] = {
		{0x107AC08050500BFFULL, ~incomplete_attack_ray::bishop[ 0], 66157},
		{0x7FFFDFDFD823FFFDULL, ~incomplete_attack_ray::bishop[ 1], 71730},
		{0x0400C00FE8000200ULL, ~incomplete_attack_ray::bishop[ 2], 37781},
		{0x103F802004000000ULL, ~incomplete_attack_ray::bishop[ 3], 21015},
		{0xC03FE00100000000ULL, ~incomplete_attack_ray::bishop[ 4], 47590},
		{0x24C00BFFFF400000ULL, ~incomplete_attack_ray::bishop[ 5],   835},
		{0x0808101F40007F04ULL, ~incomplete_attack_ray::bishop[ 6], 23592},
		{0x100808201EC00080ULL, ~incomplete_attack_ray::bishop[ 7], 30599},
		{0xFFA2FEFFBFEFB7FFULL, ~incomplete_attack_ray::bishop[ 8], 68776},
		{0x083E3EE040080801ULL, ~incomplete_attack_ray::bishop[ 9], 19959},
		{0x040180BFF7E80080ULL, ~incomplete_attack_ray::bishop[10], 21783},
		{0x0440007FE0031000ULL, ~incomplete_attack_ray::bishop[11], 64836},
		{0x2010007FFC000000ULL, ~incomplete_attack_ray::bishop[12], 23417},
		{0x1079FFE000FF8000ULL, ~incomplete_attack_ray::bishop[13], 66724},
		{0x7F83FFDFC03FFF80ULL, ~incomplete_attack_ray::bishop[14], 74542},
		{0x080614080FA00040ULL, ~incomplete_attack_ray::bishop[15], 67266},
		{0x7FFE7FFF817FCFF9ULL, ~incomplete_attack_ray::bishop[16], 26575},
		{0x7FFEBFFFA01027FDULL, ~incomplete_attack_ray::bishop[17], 67543},
		{0x20018000C00F3C01ULL, ~incomplete_attack_ray::bishop[18], 24409},
		{0x407E0001000FFB8AULL, ~incomplete_attack_ray::bishop[19], 30779},
		{0x201FE000FFF80010ULL, ~incomplete_attack_ray::bishop[20], 17384},
		{0xFFDFEFFFDE39FFEFULL, ~incomplete_attack_ray::bishop[21], 18778},
		{0x7FFFF800203FBFFFULL, ~incomplete_attack_ray::bishop[22], 65109},
		{0x7FF7FBFFF8203FFFULL, ~incomplete_attack_ray::bishop[23], 20184},
		{0x000000FE04004070ULL, ~incomplete_attack_ray::bishop[24], 38240},
		{0x7FFF7F9FFFC0EFF9ULL, ~incomplete_attack_ray::bishop[25], 16459},
		{0x7FFEFF7F7F01F7FDULL, ~incomplete_attack_ray::bishop[26], 17432},
		{0x3F6EFBBF9EFBFFFFULL, ~incomplete_attack_ray::bishop[27], 81040},
		{0x0410008F01003FFDULL, ~incomplete_attack_ray::bishop[28], 84946},
		{0x20002038001C8010ULL, ~incomplete_attack_ray::bishop[29], 18276},
		{0x087FF038000FC001ULL, ~incomplete_attack_ray::bishop[30],  8512},
		{0x00080C0C00083007ULL, ~incomplete_attack_ray::bishop[31], 78544},
		{0x00000080FC82C040ULL, ~incomplete_attack_ray::bishop[32], 19974},
		{0x000000407E416020ULL, ~incomplete_attack_ray::bishop[33], 23850},
		{0x00600203F8008020ULL, ~incomplete_attack_ray::bishop[34], 11056},
		{0xD003FEFE04404080ULL, ~incomplete_attack_ray::bishop[35], 68019},
		{0x100020801800304AULL, ~incomplete_attack_ray::bishop[36], 85965},
		{0x7FBFFE700BFFE800ULL, ~incomplete_attack_ray::bishop[37], 80524},
		{0x107FF00FE4000F90ULL, ~incomplete_attack_ray::bishop[38], 38221},
		{0x7F8FFFCFF1D007F8ULL, ~incomplete_attack_ray::bishop[39], 64647},
		{0x0000004100F88080ULL, ~incomplete_attack_ray::bishop[40], 61320},
		{0x00000020807C4040ULL, ~incomplete_attack_ray::bishop[41], 67281},
		{0x00000041018700C0ULL, ~incomplete_attack_ray::bishop[42], 79076},
		{0x0010000080FC4080ULL, ~incomplete_attack_ray::bishop[43], 17115},
		{0x1000003C80180030ULL, ~incomplete_attack_ray::bishop[44], 50718},
		{0x2006001CF00C0018ULL, ~incomplete_attack_ray::bishop[45], 24659},
		{0xFFFFFFBFEFF80FDCULL, ~incomplete_attack_ray::bishop[46], 38291},
		{0x000000101003F812ULL, ~incomplete_attack_ray::bishop[47], 30605},
		{0x0800001F40808200ULL, ~incomplete_attack_ray::bishop[48], 37759},
		{0x084000101F3FD208ULL, ~incomplete_attack_ray::bishop[49],  4639},
		{0x080000000F808081ULL, ~incomplete_attack_ray::bishop[50], 21759},
		{0x0004000008003F80ULL, ~incomplete_attack_ray::bishop[51], 67799},
		{0x08000001001FE040ULL, ~incomplete_attack_ray::bishop[52], 22841},
		{0x085F7D8000200A00ULL, ~incomplete_attack_ray::bishop[53], 66689},
		{0xFFFFFEFFBFEFF81DULL, ~incomplete_attack_ray::bishop[54], 62548},
		{0xFFBFFFEFEFDFF70FULL, ~incomplete_attack_ray::bishop[55], 66597},
		{0x100000101EC10082ULL, ~incomplete_attack_ray::bishop[56], 86749},
		{0x7FBAFFFFEFE0C02FULL, ~incomplete_attack_ray::bishop[57], 69558},
		{0x7F83FFFFFFF07F7FULL, ~incomplete_attack_ray::bishop[58], 61589},
		{0xFFF1FFFFFFF7FFC1ULL, ~incomplete_attack_ray::bishop[59], 62533},
		{0x0878040000FFE01FULL, ~incomplete_attack_ray::bishop[60], 64387},
		{0x005D00000120200AULL, ~incomplete_attack_ray::bishop[61], 26581},
		{0x0840800080200FDAULL, ~incomplete_attack_ray::bishop[62], 76355},
		{0x100000C05F582008ULL, ~incomplete_attack_ray::bishop[63], 11140},
	};

	// define the magics of the rooks
	constexpr magic_t rook[64] = {
		{0x80280013FF84FFFFULL, ~incomplete_attack_ray::rook[ 0], 10890},
		{0x5FFBFEFDFEF67FFFULL, ~incomplete_attack_ray::rook[ 1], 56054},
		{0xFFEFFAFFEFFDFFFFULL, ~incomplete_attack_ray::rook[ 2], 67495},
		{0x003000900300008AULL, ~incomplete_attack_ray::rook[ 3], 72797},
		{0x0030018003500030ULL, ~incomplete_attack_ray::rook[ 4], 17179},
		{0x0020012120A00020ULL, ~incomplete_attack_ray::rook[ 5], 63978},
		{0x0030006000C00030ULL, ~incomplete_attack_ray::rook[ 6], 56650},
		{0xFFA8008DFF09FFF8ULL, ~incomplete_attack_ray::rook[ 7], 15929},
		{0x7FBFF7FBFBEAFFFCULL, ~incomplete_attack_ray::rook[ 8], 55905},
		{0x0000140081050002ULL, ~incomplete_attack_ray::rook[ 9], 26301},
		{0x0000180043800048ULL, ~incomplete_attack_ray::rook[10], 78100},
		{0x7FFFE800021FFFB8ULL, ~incomplete_attack_ray::rook[11], 86245},
		{0xFFFFCFFE7FCFFFAFULL, ~incomplete_attack_ray::rook[12], 75228},
		{0x00001800C0180060ULL, ~incomplete_attack_ray::rook[13], 31661},
		{0xFFFFE7FF8FBFFFE8ULL, ~incomplete_attack_ray::rook[14], 38053},
		{0x0000180030620018ULL, ~incomplete_attack_ray::rook[15], 37433},
		{0x00300018010C0003ULL, ~incomplete_attack_ray::rook[16], 74747},
		{0x0003000C0085FFFFULL, ~incomplete_attack_ray::rook[17], 53847},
		{0xFFFDFFF7FBFEFFF7ULL, ~incomplete_attack_ray::rook[18], 70952},
		{0x7FC1FFDFFC001FFFULL, ~incomplete_attack_ray::rook[19], 49447},
		{0xFFFEFFDFFDFFDFFFULL, ~incomplete_attack_ray::rook[20], 62629},
		{0x7C108007BEFFF81FULL, ~incomplete_attack_ray::rook[21], 58996},
		{0x20408007BFE00810ULL, ~incomplete_attack_ray::rook[22], 36009},
		{0x0400800558604100ULL, ~incomplete_attack_ray::rook[23], 21230},
		{0x0040200010080008ULL, ~incomplete_attack_ray::rook[24], 51882},
		{0x0010020008040004ULL, ~incomplete_attack_ray::rook[25], 11841},
		{0xFFFDFEFFF7FBFFF7ULL, ~incomplete_attack_ray::rook[26], 25794},
		{0xFEBF7DFFF8FEFFF9ULL, ~incomplete_attack_ray::rook[27], 49689},
		{0xC00000FFE001FFE0ULL, ~incomplete_attack_ray::rook[28], 63400},
		{0x2008208007004007ULL, ~incomplete_attack_ray::rook[29], 33958},
		{0xBFFBFAFFFB683F7FULL, ~incomplete_attack_ray::rook[30], 21991},
		{0x0807F67FFA102040ULL, ~incomplete_attack_ray::rook[31], 45618},
		{0x200008E800300030ULL, ~incomplete_attack_ray::rook[32], 70134},
		{0x0000008780180018ULL, ~incomplete_attack_ray::rook[33], 75944},
		{0x0000010300180018ULL, ~incomplete_attack_ray::rook[34], 68392},
		{0x4000008180180018ULL, ~incomplete_attack_ray::rook[35], 66472},
		{0x008080310005FFFAULL, ~incomplete_attack_ray::rook[36], 23236},
		{0x4000188100060006ULL, ~incomplete_attack_ray::rook[37], 19067},
		{0xFFFFFF7FFFBFBFFFULL, ~incomplete_attack_ray::rook[38],     0},
		{0x0000802000200040ULL, ~incomplete_attack_ray::rook[39], 43566},
		{0x20000202EC002800ULL, ~incomplete_attack_ray::rook[40], 29810},
		{0xFFFFF9FF7CFFF3FFULL, ~incomplete_attack_ray::rook[41], 65558},
		{0x000000404B801800ULL, ~incomplete_attack_ray::rook[42], 77684},
		{0x2000002FE03FD000ULL, ~incomplete_attack_ray::rook[43], 73350},
		{0xFFFFFF6FFE7FCFFDULL, ~incomplete_attack_ray::rook[44], 61765},
		{0xBFF7EFFFBFC00FFFULL, ~incomplete_attack_ray::rook[45], 49282},
		{0x000000100800A804ULL, ~incomplete_attack_ray::rook[46], 78840},
		{0xFFFBFFEFA7FFA7FEULL, ~incomplete_attack_ray::rook[47], 82904},
		{0x0000052800140028ULL, ~incomplete_attack_ray::rook[48], 24594},
		{0x00000085008A0014ULL, ~incomplete_attack_ray::rook[49],  9513},
		{0x8000002B00408028ULL, ~incomplete_attack_ray::rook[50], 29012},
		{0x4000002040790028ULL, ~incomplete_attack_ray::rook[51], 27684},
		{0x7800002010288028ULL, ~incomplete_attack_ray::rook[52], 27901},
		{0x0000001800E08018ULL, ~incomplete_attack_ray::rook[53], 61477},
		{0x1890000810580050ULL, ~incomplete_attack_ray::rook[54], 25719},
		{0x2003D80000500028ULL, ~incomplete_attack_ray::rook[55], 50020},
		{0xFFFFF37EEFEFDFBEULL, ~incomplete_attack_ray::rook[56], 41547},
		{0x40000280090013C1ULL, ~incomplete_attack_ray::rook[57],  4750},
		{0xBF7FFEFFBFFAF71FULL, ~incomplete_attack_ray::rook[58],  6014},
		{0xFFFDFFFF777B7D6EULL, ~incomplete_attack_ray::rook[59], 41529},
		{0xEEFFFFEFF0080BFEULL, ~incomplete_attack_ray::rook[60], 84192},
		{0xAFE0000FFF780402ULL, ~incomplete_attack_ray::rook[61], 33433},
		{0xEE73FFFBFFBB77FEULL, ~incomplete_attack_ray::rook[62],  8555},
		{0x0002000308482882ULL, ~incomplete_attack_ray::rook[63],  1009},
	};

	// generate the magic table from the magics
	constexpr std::array<bitboard_t, 88507> table = {[]() constexpr {
		std::array<bitboard_t, 88507> table{bitboard::none};
		for (square_t square = 0; square < 64; square++) {
			bitboard_t bishop_attack_ray = incomplete_attack_ray::bishop[square];
			int occupancy_bound_bishop = 1 << popcount(bishop_attack_ray);
			for (int occupancy_index = 0; occupancy_index < occupancy_bound_bishop; occupancy_index++) {
				bitboard_t occupancy = generate_occupancy(occupancy_index, bishop_attack_ray);
				table[
					bishop[square].offset +
					(((occupancy | bishop[square].mask) * bishop[square].magic_number) >> (64 - 9))
				] = attack::slow::bishop(square, occupancy);
			};
			bitboard_t rook_attack_ray = incomplete_attack_ray::rook[square];
			int occupancy_bound_rook = 1 << popcount(rook_attack_ray);
			for (int occupancy_index = 0; occupancy_index < occupancy_bound_rook; occupancy_index++) {
				bitboard_t occupancy = generate_occupancy(occupancy_index, rook_attack_ray);
				table[
					rook[square].offset +
					(((occupancy | rook[square].mask) * rook[square].magic_number) >> (64 - 12))
				] = attack::slow::rook(square, occupancy);
			};
		};
		return table;
	}()};
};


#endif // __MAGIC__MODULE__