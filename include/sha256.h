#ifndef HEADER_SHA256_H
#define HEADER_SHA256_H

#include "../libft/libft.h"

/* 
	SHA2 RFC: https://www.rfc-editor.org/rfc/rfc4634.html#section-5.3.2
	Initial Hash Values: FIPS-180-2 section 5.3.2 
	static uint32_t SHA256_H0[SHA256HashSize/4] = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
	};
*/

#define SHA256_REGISTER_INIT	\
	((u32){0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19})

/* Define the SHA shift, rotate left and rotate right macro */

/* Shift right */
#define SHR(bits,word)	((word) >> (bits))

/* Circular rotate left */
#define ROTL(bits,word)	(((word) << (bits)) | ((word) >> (32-(bits))))

/* Cicurale rotate right */
#define ROTR(bits,word) (((word) >> (bits)) | ((word) << (32-(bits))))

/* Define the SHA SIGMA and sigma macros */
/**
 *	Macro function descripte by the RFC 4634         
 *	CH( x, y, z) = (x AND y) XOR ( (NOT x) AND z)
 * 	MAJ( x, y, z) = (x AND y) XOR (x AND z) XOR (y AND z)
 *	BSIG0(x) = ROTR^2(x) XOR ROTR^13(x) XOR ROTR^22(x)
 *	BSIG1(x) = ROTR^6(x) XOR ROTR^11(x) XOR ROTR^25(x)
 *	SSIG0(x) = ROTR^7(x) XOR ROTR^18(x) XOR SHR^3(x)
 *	SSIG1(x) = ROTR^17(x) XOR ROTR^19(x) XOR SHR^10(x)
*/

/* Caps SIGMA represent by 'Σ' */
#define BSIG0(word)	(SHA256_ROTR( 2,word) ^ SHA256_ROTR(13,word) ^ SHA256_ROTR(22,word))
#define BSIG1(word)	(SHA256_ROTR( 6,word) ^ SHA256_ROTR(11,word) ^ SHA256_ROTR(25,word))

/* minus sigma represent by 'σ' */
#define SSIG0(word)	(SHA256_ROTR( 7,word) ^ SHA256_ROTR(18,word) ^ SHA256_SHR( 3,word))
#define SSIG1(word)	(SHA256_ROTR(17,word) ^ SHA256_ROTR(19,word) ^ SHA256_SHR(10,word))


/* CH and MAJ are the basic SHA-256 functions */

/* Ch (Choose function)*/
#define CH(x,y,z)	(((x) & (y)) ^ (~(x) & (z)))

/* Maj (Majority function) */
#define MAJ(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


#endif /* HEADER_SHA256_H */
