#ifndef WHIRPOOL_H
#define WHIRPOOL_H

#include "../libft/libft.h"

typedef struct WhirlpoolCtx {
	u64 h[8];		// Hash value
	u64 x[8];		// Hash value
	u64 k[8];		// Key
	u64 l[8];		// Length
	u64 state[8];	// State
	u8 buffer[BYTES_BLOCK_SIZE];	// Buffer
} WhirlpoolCtx;

/**
 * @brief Rotate bits to the right
 * @param val value to rotate
 * @param shift number of bits to shift
 * @return rotated value
*/
#define ROR64(val, shift) (((val) >> (shift)) | ((val) << (64 - (shift))))

//Round function
#define RHO(b, a, n, c) \
{ \
   b = t[(a[n] >> 56) & 0xFF]; \
   b ^= ROR64(t[(a[(n + 7) % 8] >> 48) & 0xFF], 8); \
   b ^= ROR64(t[(a[(n + 6) % 8] >> 40) & 0xFF], 16); \
   b ^= ROR64(t[(a[(n + 5) % 8] >> 32) & 0xFF], 24); \
   b ^= ROR64(t[(a[(n + 4) % 8] >> 24) & 0xFF], 32); \
   b ^= ROR64(t[(a[(n + 3) % 8] >> 16) & 0xFF], 40); \
   b ^= ROR64(t[(a[(n + 2) % 8] >> 8) & 0xFF], 48); \
   b ^= ROR64(t[a[(n + 1) % 8] & 0xFF], 56); \
   b ^= c; \
}

#define WHIRLPOOL_DIGEST_SIZE 32

#define LOAD64BE(p) ( \
   ((uint64_t)(((uint8_t *)(p))[0]) << 56) | \
   ((uint64_t)(((uint8_t *)(p))[1]) << 48) | \
   ((uint64_t)(((uint8_t *)(p))[2]) << 40) | \
   ((uint64_t)(((uint8_t *)(p))[3]) << 32) | \
   ((uint64_t)(((uint8_t *)(p))[4]) << 24) | \
   ((uint64_t)(((uint8_t *)(p))[5]) << 16) | \
   ((uint64_t)(((uint8_t *)(p))[6]) << 8) | \
   ((uint64_t)(((uint8_t *)(p))[7]) << 0))

#define STORE64BE(a, p) \
((uint8_t *)(p))[0] = ((uint64_t)(a) >> 56) & 0xFFU, \
((uint8_t *)(p))[1] = ((uint64_t)(a) >> 48) & 0xFFU, \
((uint8_t *)(p))[2] = ((uint64_t)(a) >> 40) & 0xFFU, \
((uint8_t *)(p))[3] = ((uint64_t)(a) >> 32) & 0xFFU, \
((uint8_t *)(p))[4] = ((uint64_t)(a) >> 24) & 0xFFU, \
((uint8_t *)(p))[5] = ((uint64_t)(a) >> 16) & 0xFFU, \
((uint8_t *)(p))[6] = ((uint64_t)(a) >> 8) & 0xFFU, \
((uint8_t *)(p))[7] = ((uint64_t)(a) >> 0) & 0xFFU

void whirlpool_set_context(HashCtx *ctx);
void whirlpool_hash(HashCtx *ctx, u8 *str, u64 len);

#endif /* WHIRLPOOL_H */