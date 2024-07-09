#include "../include/sha256.h"

void sha256_transform(u32 *state, u8 *block, u32 k[64]) {
    u32 a, b, c, d, e, f, g, h;		/* Register working variable */
    u32 w[64];						/* Message schedule (word) */
    u32 tmp1, tmp2;					/* Temporary variable */
    s32 t;							/* Loop counter */

    /* Save register */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];

    // for (t = 0; t < 16; ++t) {
    //     w[t] = ((u32)block[t * 4] << 24) | ((u32)block[t * 4 + 1] << 16) |
    //            ((u32)block[t * 4 + 2] << 8) | ((u32)block[t * 4 + 3]);
    // }
	/* Fill the 16 first word value (message schedule) */
	block_to_u32(block, w);
    for (t = 16; t < 64; ++t) {
        w[t] = SSIG1(w[t - 2]) + w[t - 7] + SSIG0(w[t - 15]) + w[t - 16];
    }

    /* Perform the main hash computation */
    for (t = 0; t < 64; ++t) {
        tmp1 = h + BSIG1(e) + CH(e, f, g) + k[t] + w[t];
        tmp2 = BSIG0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + tmp1;
        d = c;
        c = b;
        b = a;
        a = tmp1 + tmp2;
    }

    /* Add the working variables back into the current hash value */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}
