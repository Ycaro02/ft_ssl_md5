#ifndef HEADER_FT_SSL_H
#define HEADER_FT_SSL_H

#include "../libft/libft.h"

/** 
 * From RFC 1321 - The MD5 Message-Digest Algorithm: https://www.ietf.org/rfc/rfc1321.txt
 * Register first state:
 * 		Hexa:
 *		word A: 01 23 45 67
 *		word B: 89 ab cd ef
 *		word C: fe dc ba 98
 *		word D: 76 54 32 10
 */

#define COMPUTED_RA_HEX ((0x67 << 24) + (0x45 << 16) + (0x23 << 8) + 0x01)
#define COMPUTED_RB_HEX ((0xef << 24) + (0xcd << 16) + (0xab << 8) + 0x89)
#define COMPUTED_RC_HEX ((0x98 << 24) + (0xba << 16) + (0xdc << 8) + 0xfe)
#define COMPUTED_RD_HEX ((0x10 << 24) + (0x32 << 16) + (0x54 << 8) + 0x76)

#define RA_HEX (0x67452301)
#define RB_HEX (0xefcdab89)
#define RC_HEX (0x98badcfe)
#define RD_HEX (0x10325476)

#define RA_BIN "01100111 01000101 00100011 00000001"
#define RB_BIN "11101111 11001101 10101011 10001001"
#define RC_BIN "10011000 10111010 11011100 11111110"
#define RD_BIN "01110110 01010100 00110010 00010000"

/**
 * Function for iterate on byte block
 *	F(X,Y,Z) = XY v not(X) Z
 *	G(X,Y,Z) = XZ v Y not(Z)
 *	H(X,Y,Z) = X xor Y xor Z
 *	I(X,Y,Z) = Y xor (X v not(Z))
 *	Implementation with bitwise operator:
 *	F = (X & Y) | ((~X) & Z)
 *	G = (X & Z) | (Y & (~Z))
 *	H = X ^ Y ^ Z
 *	I = Y ^ (X | (~Z))
*/

#define FUNCT_F(X, Y, Z)    ((X & Y) | ((~X) & Z))
#define FUNCT_G(X, Y, Z)    ((X & Z) | (Y & (~Z)))
#define FUNCT_H(X, Y, Z)    (X ^ Y ^ Z)
#define FUNCT_I(X, Y, Z)    (Y ^ (X | (~Z)))

/* Define MD5 block size */
#define MD5_BLOCK_SIZE ((u32)512U)

/* Define MD5 last block size, 512 - len on 64 - 1 cause we need at least one padding bits  */
#define MD5_LAST_BLOCK_SIZE ((u32)(512U - 64U - 1U))

/* Define order we use M data for each round */
#define FIRST_ROUND_ORDER	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
#define SECOND_ROUND_ORDER	{ 1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12 }
#define THIRD_ROUND_ORDER	{ 5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2 }
#define FOURTH_ROUND_ORDER	{ 0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9 }

#define M_DATA_SIZE		16

typedef struct s_md5_context {
	char	*input;				/* Input string */
	t_list	*block_list;		/* List of binary block str */
	
	/*	
		Splited block, (M data) of 16 uint (512 / 16 == 16 4 bytes word) for each block
		first idx is block idx and second is splited data 
	*/
	u32		**splited_block;	/* Splited block */
	u32		list_size;			/* Size of block list */
	u32		input_size;			/* Size of input string */
	u32		A;					/* Buffer A */
	u32		B;					/* Buffer B */
	u32		C;					/* Buffer C */
	u32		D;					/* Buffer D */
} MD5_Context;

/* binary_utils.c */
char	*char_to_binary(char c);
char	*u64_to_binary(u64 n);
char	*string_to_binary(char *str);
char	*build_binary_block(char *str, u64 base_len, s8 last_block);
t_list	*string_to_binary_block_list(char *str);
/* atoi_base.c */
int		ft_atoi_base(char *str, char *base);

#endif /* HEADER_FT_SSL_H */
