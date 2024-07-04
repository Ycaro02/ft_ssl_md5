#ifndef HEADER_FT_SSL_H
#define HEADER_FT_SSL_H

#include "../libft/libft.h"


/* MD5 */

/** 
 * From RFC 1321 - The MD5 Message-Digest Algorithm: https://www.ietf.org/rfc/rfc1321.txt
 * Register first state:
 * 		Hexa:
 *		word A: 01 23 45 67
 *		word B: 89 ab cd ef
 *		word C: fe dc ba 98
 *		word D: 76 54 32 10
#define COMPUTED_RA_HEX ((0x67U << 24) + (0x45U << 16) + (0x23U << 8) + 0x01U)
#define COMPUTED_RB_HEX ((0xefU << 24) + (0xcdU << 16) + (0xabU << 8) + 0x89U)
#define COMPUTED_RC_HEX ((0x98U << 24) + (0xbaU << 16) + (0xdcU << 8) + 0xfeU)
#define COMPUTED_RD_HEX ((0x10U << 24) + (0x32U << 16) + (0x54U << 8) + 0x76U)
 */


#define RA_HEX (0x67452301U)
#define RB_HEX (0xefcdab89U)
#define RC_HEX (0x98badcfeU)
#define RD_HEX (0x10325476U)

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

// #define FUNCT_F(X, Y, Z)    ((X & Y) | ((~X) & Z))
// #define FUNCT_G(X, Y, Z)    ((X & Z) | (Y & (~Z)))
// #define FUNCT_H(X, Y, Z)    (X ^ Y ^ Z)
// #define FUNCT_I(X, Y, Z)    (Y ^ (X | (~Z)))


/* Define MD5 block size */
#define MD5_BLOCK_SIZE ((u32)512U)

/* Define MD5 last block size, 512 - len on 64 - 1 cause we need at least one padding bits  */
#define MD5_LAST_BLOCK_SIZE ((u32)(512U - 64U - 1U))

/* Number of M data (word) per block */
#define M_DATA_SIZE		16

// /* Define order we use M data for each round */
// #define FIRST_ROUND_ORDER	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
// #define SECOND_ROUND_ORDER	{ 1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12 }
// #define THIRD_ROUND_ORDER	{ 5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2 }
// #define FOURTH_ROUND_ORDER	{ 0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9 }

// #define M_SELECTION_ORDER	{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12,5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2,0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9 }

/* Define shift value S */
#define SHIFT_ARRAY_SIZE	4
/* We need to use them with the mod operator on the i number operation i % 4 */
// #define MD5_SHIFT1	((s32 [4]){ 7, 12, 17, 22 })	/* Round 1 */
// #define MD5_SHIFT2	((s32 [4]){ 5, 9, 14, 20 })	/* Round 2 */
// #define MD5_SHIFT3	((s32 [4]){ 4, 11, 16, 23 })	/* Round 3 */
// #define MD5_SHIFT4	((s32 [4]){ 6, 10, 15, 21 })	/* Round 4 */

typedef struct s_md5_context {
	u32		K[64];				/* K constant */
	char	*input;				/* Input string */
	char	*binary_input;		/* Input string in binary */
	t_list	*block_list;		/* List of binary block str */
	
	/*	
		Splited block, (M data) of 16 uint (512 / 16 == 16 4 bytes word) for each block
		first idx is block idx and second is splited data 
	*/
	u32		**splited_block;	/* Splited block (M word for each block) */
	u32		list_size;			/* Size of block list */
	u32		input_size;			/* Size of input string */
	u32		binary_input_size;	/* Size of input string in binary */
	u32		A;					/* Buffer A */
	u32		B;					/* Buffer B */
	u32		C;					/* Buffer C */
	u32		D;					/* Buffer D */
} MD5_Context;

/* binary_utils.c */
char	*char_to_binary(char c);
char	*u64_to_binary(u64 n);
char	*string_to_binary(char *str);
// char	*build_binary_block(char *str, u64 base_len, s8 last_block);
t_list	*string_to_binary_block_list(char *str);
void	MD5_process(char *input);
/* atoi_base.c */
int		ft_atoi_base(char *str, char *base);

#endif /* HEADER_FT_SSL_H */
