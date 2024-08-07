#ifndef HEADER_MD5_H
#define HEADER_MD5_H

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
*/

#define RA_HEX (0x67452301)
#define RB_HEX (0xefcdab89)
#define RC_HEX (0x98badcfe)
#define RD_HEX (0x10325476)

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

FT_INLINE u32	func_f(u32 b, u32 c, u32 d) { return ((b & c) | ((~b) & d)); }
FT_INLINE u32	func_g(u32 b, u32 c, u32 d) { return ((d & b) | ((~d) & c)); }
FT_INLINE u32	func_h(u32 b, u32 c, u32 d) { return (b ^ c ^ d); }
FT_INLINE u32	func_i(u32 b, u32 c, u32 d) { return (c ^ (b | (~d))); }

/* Number of M data (word) per block a word is a 32 bits unsigne integer */
#define MD5_NB_WORD	16

/* Number of iteration for each block */
#define MD5_IT_NB	64

/* Size of MD5 digest in bytes */
#define MD5_DIGEST_SIZE	16

/* Shift value for each round */
#define MD5_SHIFT1	((u32 [4]){ 7, 12, 17, 22 })	/* Round 1 */
#define MD5_SHIFT2	((u32 [4]){ 5, 9, 14, 20 })		/* Round 2 */
#define MD5_SHIFT3	((u32 [4]){ 4, 11, 16, 23 })	/* Round 3 */
#define MD5_SHIFT4	((u32 [4]){ 6, 10, 15, 21 })	/* Round 4 */

/* md5.c*/
void	MD5_set_context(HashCtx *ctx);

#endif /* HEADER_MD5_H */
