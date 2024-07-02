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

#define RA_HEX 0x67452301
#define RB_HEX 0xefcdab89
#define RC_HEX 0x98badcfe
#define RD_HEX 0x10325476

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

/* binary_utils.c */
char	*char_to_binary(char c);

/* atoi_base.c */
int		ft_atoi_base(char *str, char *base);

#endif /* HEADER_FT_SSL_H */
