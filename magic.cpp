#ifdef __INTELLISENSE__
  #pragma diag_suppress 28
#endif


#include "attack_rays.cpp"
#include "constants.cpp"
#include "correct_attacks.cpp"


#ifndef __MAGIC__MODULE__
#define __MAGIC__MODULE__


namespace MAGIC {
	// generates possible occupancies on attack rays
	static constexpr bitboard_t generate_occupancy(int index, bitboard_t attack_ray) {
		bitboard_t occupancy = BITBOARD::none;
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
		U64 magic_number;
		bitboard_t mask;
		int offset;
	};

	// define the magics of the bishop
	static const magic_t bishop[64] = {
		{0x107AC08050500BFFULL, ~ATTACK_RAY::bishop_short[ 0], 66157},
		{0x7FFFDFDFD823FFFDULL, ~ATTACK_RAY::bishop_short[ 1], 71730},
		{0x0400C00FE8000200ULL, ~ATTACK_RAY::bishop_short[ 2], 37781},
		{0x103F802004000000ULL, ~ATTACK_RAY::bishop_short[ 3], 21015},
		{0xC03FE00100000000ULL, ~ATTACK_RAY::bishop_short[ 4], 47590},
		{0x24C00BFFFF400000ULL, ~ATTACK_RAY::bishop_short[ 5],   835},
		{0x0808101F40007F04ULL, ~ATTACK_RAY::bishop_short[ 6], 23592},
		{0x100808201EC00080ULL, ~ATTACK_RAY::bishop_short[ 7], 30599},
		{0xFFA2FEFFBFEFB7FFULL, ~ATTACK_RAY::bishop_short[ 8], 68776},
		{0x083E3EE040080801ULL, ~ATTACK_RAY::bishop_short[ 9], 19959},
		{0x040180BFF7E80080ULL, ~ATTACK_RAY::bishop_short[10], 21783},
		{0x0440007FE0031000ULL, ~ATTACK_RAY::bishop_short[11], 64836},
		{0x2010007FFC000000ULL, ~ATTACK_RAY::bishop_short[12], 23417},
		{0x1079FFE000FF8000ULL, ~ATTACK_RAY::bishop_short[13], 66724},
		{0x7F83FFDFC03FFF80ULL, ~ATTACK_RAY::bishop_short[14], 74542},
		{0x080614080FA00040ULL, ~ATTACK_RAY::bishop_short[15], 67266},
		{0x7FFE7FFF817FCFF9ULL, ~ATTACK_RAY::bishop_short[16], 26575},
		{0x7FFEBFFFA01027FDULL, ~ATTACK_RAY::bishop_short[17], 67543},
		{0x20018000C00F3C01ULL, ~ATTACK_RAY::bishop_short[18], 24409},
		{0x407E0001000FFB8AULL, ~ATTACK_RAY::bishop_short[19], 30779},
		{0x201FE000FFF80010ULL, ~ATTACK_RAY::bishop_short[20], 17384},
		{0xFFDFEFFFDE39FFEFULL, ~ATTACK_RAY::bishop_short[21], 18778},
		{0x7FFFF800203FBFFFULL, ~ATTACK_RAY::bishop_short[22], 65109},
		{0x7FF7FBFFF8203FFFULL, ~ATTACK_RAY::bishop_short[23], 20184},
		{0x000000FE04004070ULL, ~ATTACK_RAY::bishop_short[24], 38240},
		{0x7FFF7F9FFFC0EFF9ULL, ~ATTACK_RAY::bishop_short[25], 16459},
		{0x7FFEFF7F7F01F7FDULL, ~ATTACK_RAY::bishop_short[26], 17432},
		{0x3F6EFBBF9EFBFFFFULL, ~ATTACK_RAY::bishop_short[27], 81040},
		{0x0410008F01003FFDULL, ~ATTACK_RAY::bishop_short[28], 84946},
		{0x20002038001C8010ULL, ~ATTACK_RAY::bishop_short[29], 18276},
		{0x087FF038000FC001ULL, ~ATTACK_RAY::bishop_short[30],  8512},
		{0x00080C0C00083007ULL, ~ATTACK_RAY::bishop_short[31], 78544},
		{0x00000080FC82C040ULL, ~ATTACK_RAY::bishop_short[32], 19974},
		{0x000000407E416020ULL, ~ATTACK_RAY::bishop_short[33], 23850},
		{0x00600203F8008020ULL, ~ATTACK_RAY::bishop_short[34], 11056},
		{0xD003FEFE04404080ULL, ~ATTACK_RAY::bishop_short[35], 68019},
		{0x100020801800304AULL, ~ATTACK_RAY::bishop_short[36], 85965},
		{0x7FBFFE700BFFE800ULL, ~ATTACK_RAY::bishop_short[37], 80524},
		{0x107FF00FE4000F90ULL, ~ATTACK_RAY::bishop_short[38], 38221},
		{0x7F8FFFCFF1D007F8ULL, ~ATTACK_RAY::bishop_short[39], 64647},
		{0x0000004100F88080ULL, ~ATTACK_RAY::bishop_short[40], 61320},
		{0x00000020807C4040ULL, ~ATTACK_RAY::bishop_short[41], 67281},
		{0x00000041018700C0ULL, ~ATTACK_RAY::bishop_short[42], 79076},
		{0x0010000080FC4080ULL, ~ATTACK_RAY::bishop_short[43], 17115},
		{0x1000003C80180030ULL, ~ATTACK_RAY::bishop_short[44], 50718},
		{0x2006001CF00C0018ULL, ~ATTACK_RAY::bishop_short[45], 24659},
		{0xFFFFFFBFEFF80FDCULL, ~ATTACK_RAY::bishop_short[46], 38291},
		{0x000000101003F812ULL, ~ATTACK_RAY::bishop_short[47], 30605},
		{0x0800001F40808200ULL, ~ATTACK_RAY::bishop_short[48], 37759},
		{0x084000101F3FD208ULL, ~ATTACK_RAY::bishop_short[49],  4639},
		{0x080000000F808081ULL, ~ATTACK_RAY::bishop_short[50], 21759},
		{0x0004000008003F80ULL, ~ATTACK_RAY::bishop_short[51], 67799},
		{0x08000001001FE040ULL, ~ATTACK_RAY::bishop_short[52], 22841},
		{0x085F7D8000200A00ULL, ~ATTACK_RAY::bishop_short[53], 66689},
		{0xFFFFFEFFBFEFF81DULL, ~ATTACK_RAY::bishop_short[54], 62548},
		{0xFFBFFFEFEFDFF70FULL, ~ATTACK_RAY::bishop_short[55], 66597},
		{0x100000101EC10082ULL, ~ATTACK_RAY::bishop_short[56], 86749},
		{0x7FBAFFFFEFE0C02FULL, ~ATTACK_RAY::bishop_short[57], 69558},
		{0x7F83FFFFFFF07F7FULL, ~ATTACK_RAY::bishop_short[58], 61589},
		{0xFFF1FFFFFFF7FFC1ULL, ~ATTACK_RAY::bishop_short[59], 62533},
		{0x0878040000FFE01FULL, ~ATTACK_RAY::bishop_short[60], 64387},
		{0x005D00000120200AULL, ~ATTACK_RAY::bishop_short[61], 26581},
		{0x0840800080200FDAULL, ~ATTACK_RAY::bishop_short[62], 76355},
		{0x100000C05F582008ULL, ~ATTACK_RAY::bishop_short[63], 11140},
	};

	// define the magics of the rooks
	static const magic_t rook[64] = {
		{0x80280013FF84FFFFULL, ~ATTACK_RAY::rook_short[ 0], 10890},
		{0x5FFBFEFDFEF67FFFULL, ~ATTACK_RAY::rook_short[ 1], 56054},
		{0xFFEFFAFFEFFDFFFFULL, ~ATTACK_RAY::rook_short[ 2], 67495},
		{0x003000900300008AULL, ~ATTACK_RAY::rook_short[ 3], 72797},
		{0x0030018003500030ULL, ~ATTACK_RAY::rook_short[ 4], 17179},
		{0x0020012120A00020ULL, ~ATTACK_RAY::rook_short[ 5], 63978},
		{0x0030006000C00030ULL, ~ATTACK_RAY::rook_short[ 6], 56650},
		{0xFFA8008DFF09FFF8ULL, ~ATTACK_RAY::rook_short[ 7], 15929},
		{0x7FBFF7FBFBEAFFFCULL, ~ATTACK_RAY::rook_short[ 8], 55905},
		{0x0000140081050002ULL, ~ATTACK_RAY::rook_short[ 9], 26301},
		{0x0000180043800048ULL, ~ATTACK_RAY::rook_short[10], 78100},
		{0x7FFFE800021FFFB8ULL, ~ATTACK_RAY::rook_short[11], 86245},
		{0xFFFFCFFE7FCFFFAFULL, ~ATTACK_RAY::rook_short[12], 75228},
		{0x00001800C0180060ULL, ~ATTACK_RAY::rook_short[13], 31661},
		{0xFFFFE7FF8FBFFFE8ULL, ~ATTACK_RAY::rook_short[14], 38053},
		{0x0000180030620018ULL, ~ATTACK_RAY::rook_short[15], 37433},
		{0x00300018010C0003ULL, ~ATTACK_RAY::rook_short[16], 74747},
		{0x0003000C0085FFFFULL, ~ATTACK_RAY::rook_short[17], 53847},
		{0xFFFDFFF7FBFEFFF7ULL, ~ATTACK_RAY::rook_short[18], 70952},
		{0x7FC1FFDFFC001FFFULL, ~ATTACK_RAY::rook_short[19], 49447},
		{0xFFFEFFDFFDFFDFFFULL, ~ATTACK_RAY::rook_short[20], 62629},
		{0x7C108007BEFFF81FULL, ~ATTACK_RAY::rook_short[21], 58996},
		{0x20408007BFE00810ULL, ~ATTACK_RAY::rook_short[22], 36009},
		{0x0400800558604100ULL, ~ATTACK_RAY::rook_short[23], 21230},
		{0x0040200010080008ULL, ~ATTACK_RAY::rook_short[24], 51882},
		{0x0010020008040004ULL, ~ATTACK_RAY::rook_short[25], 11841},
		{0xFFFDFEFFF7FBFFF7ULL, ~ATTACK_RAY::rook_short[26], 25794},
		{0xFEBF7DFFF8FEFFF9ULL, ~ATTACK_RAY::rook_short[27], 49689},
		{0xC00000FFE001FFE0ULL, ~ATTACK_RAY::rook_short[28], 63400},
		{0x2008208007004007ULL, ~ATTACK_RAY::rook_short[29], 33958},
		{0xBFFBFAFFFB683F7FULL, ~ATTACK_RAY::rook_short[30], 21991},
		{0x0807F67FFA102040ULL, ~ATTACK_RAY::rook_short[31], 45618},
		{0x200008E800300030ULL, ~ATTACK_RAY::rook_short[32], 70134},
		{0x0000008780180018ULL, ~ATTACK_RAY::rook_short[33], 75944},
		{0x0000010300180018ULL, ~ATTACK_RAY::rook_short[34], 68392},
		{0x4000008180180018ULL, ~ATTACK_RAY::rook_short[35], 66472},
		{0x008080310005FFFAULL, ~ATTACK_RAY::rook_short[36], 23236},
		{0x4000188100060006ULL, ~ATTACK_RAY::rook_short[37], 19067},
		{0xFFFFFF7FFFBFBFFFULL, ~ATTACK_RAY::rook_short[38],     0},
		{0x0000802000200040ULL, ~ATTACK_RAY::rook_short[39], 43566},
		{0x20000202EC002800ULL, ~ATTACK_RAY::rook_short[40], 29810},
		{0xFFFFF9FF7CFFF3FFULL, ~ATTACK_RAY::rook_short[41], 65558},
		{0x000000404B801800ULL, ~ATTACK_RAY::rook_short[42], 77684},
		{0x2000002FE03FD000ULL, ~ATTACK_RAY::rook_short[43], 73350},
		{0xFFFFFF6FFE7FCFFDULL, ~ATTACK_RAY::rook_short[44], 61765},
		{0xBFF7EFFFBFC00FFFULL, ~ATTACK_RAY::rook_short[45], 49282},
		{0x000000100800A804ULL, ~ATTACK_RAY::rook_short[46], 78840},
		{0xFFFBFFEFA7FFA7FEULL, ~ATTACK_RAY::rook_short[47], 82904},
		{0x0000052800140028ULL, ~ATTACK_RAY::rook_short[48], 24594},
		{0x00000085008A0014ULL, ~ATTACK_RAY::rook_short[49],  9513},
		{0x8000002B00408028ULL, ~ATTACK_RAY::rook_short[50], 29012},
		{0x4000002040790028ULL, ~ATTACK_RAY::rook_short[51], 27684},
		{0x7800002010288028ULL, ~ATTACK_RAY::rook_short[52], 27901},
		{0x0000001800E08018ULL, ~ATTACK_RAY::rook_short[53], 61477},
		{0x1890000810580050ULL, ~ATTACK_RAY::rook_short[54], 25719},
		{0x2003D80000500028ULL, ~ATTACK_RAY::rook_short[55], 50020},
		{0xFFFFF37EEFEFDFBEULL, ~ATTACK_RAY::rook_short[56], 41547},
		{0x40000280090013C1ULL, ~ATTACK_RAY::rook_short[57],  4750},
		{0xBF7FFEFFBFFAF71FULL, ~ATTACK_RAY::rook_short[58],  6014},
		{0xFFFDFFFF777B7D6EULL, ~ATTACK_RAY::rook_short[59], 41529},
		{0xEEFFFFEFF0080BFEULL, ~ATTACK_RAY::rook_short[60], 84192},
		{0xAFE0000FFF780402ULL, ~ATTACK_RAY::rook_short[61], 33433},
		{0xEE73FFFBFFBB77FEULL, ~ATTACK_RAY::rook_short[62],  8555},
		{0x0002000308482882ULL, ~ATTACK_RAY::rook_short[63],  1009},
	};

	// generate the magic table from the magics
	static constexpr std::array<bitboard_t, 88507> generate_table() {
		std::array<bitboard_t, 88507> table{BITBOARD::none};
		for (square_t square = 0; square < 64; square++) {
			bitboard_t bishop_short_attack_ray = ATTACK_RAY::bishop_short[square];
			int occupancy_bound_bishop = 1 << popcount(bishop_short_attack_ray);
			for (int occupancy_index = 0; occupancy_index < occupancy_bound_bishop; occupancy_index++) {
				bitboard_t occupancy = generate_occupancy(occupancy_index, bishop_short_attack_ray);
				table[
					bishop[square].offset +
					(((occupancy | bishop[square].mask) * bishop[square].magic_number) >> (64 - 9))
				] = CORRECT_ATTACK::bishop(square, occupancy);
			};
			bitboard_t rook_short_attack_ray = ATTACK_RAY::rook_short[square];
			int occupancy_bound_rook = 1 << popcount(rook_short_attack_ray);
			for (int occupancy_index = 0; occupancy_index < occupancy_bound_rook; occupancy_index++) {
				bitboard_t occupancy = generate_occupancy(occupancy_index, rook_short_attack_ray);
				table[
					rook[square].offset +
					(((occupancy | rook[square].mask) * rook[square].magic_number) >> (64 - 12))
				] = CORRECT_ATTACK::rook(square, occupancy);
			};
		};
		return table;
	};

	static constexpr std::array<bitboard_t, 88507> table = generate_table();
};


#endif