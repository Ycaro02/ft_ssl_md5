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

#define SHA256_R0 0x6A09E667
#define SHA256_R1 0xBB67AE85
#define SHA256_R2 0x3C6EF372
#define SHA256_R3 0xA54FF53A
#define SHA256_R4 0x510E527F
#define SHA256_R5 0x9B05688C
#define SHA256_R6 0x1F83D9AB
#define SHA256_R7 0x5BE0CD19


// #define SHA256_REGISTER_INIT	{0x6A09E667, 0xBB67AE85, 0x3C6EF372, 
// 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19}

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
#define BSIG0(word)	(ROTR( 2,word) ^ ROTR(13,word) ^ ROTR(22,word))
#define BSIG1(word)	(ROTR( 6,word) ^ ROTR(11,word) ^ ROTR(25,word))

/* minus sigma represent by 'σ' */
#define SSIG0(word)	(ROTR( 7,word) ^ ROTR(18,word) ^ SHR( 3,word))
#define SSIG1(word)	(ROTR(17,word) ^ ROTR(19,word) ^ SHR(10,word))


/* CH and MAJ are the basic SHA-256 functions */

/* Ch (Choose function)*/
#define CH(x,y,z)	(((x) & (y)) ^ (~(x) & (z)))

/* Maj (Majority function) */
#define MAJ(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


/**
 *	SHA-224 and SHA-256 use the same sequence of sixty-four constant
 *	32-bit words, K0, K1, ..., K63.  These words represent the first
 *	thirty-two bits of the fractional parts of the cube roots of the
 *	first sixty-four prime numbers.  In hex, these constant words are as
 *	follows (from left to right):
*/

#define SHA256_K { \
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,\
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,\
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,\
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,\
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,\
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,\
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,\
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,\
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,\
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,\
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,\
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,\
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,\
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,\
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,\
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2\
}

/* Size of SHA-256 digest in bytes */
#define SHA256_DIGEST_SIZE 32

typedef struct s_sha256_ctx {
	u8		*input;		/* Input string */
	t_list	*block_list;	/* List of binary block str */

	// u32 	**splited_block;	/* Splited block (M word for each block) */
	u32		list_size;		/* Size of block list */
	u32		input_size;		/* Size of input string */
	u32		state[8];		/* State of the hash */
} SHA256_Ctx;

void SHA256_set_context(HashCtx *ctx);
void SHA256_hash_str(HashCtx *ctx, u8 *str, u64 len);
s8 SHA256_hash_file(HashCtx *ctx, char *path);
#endif /* HEADER_SHA256_H */
