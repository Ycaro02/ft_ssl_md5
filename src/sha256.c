#include "../include/ft_ssl.h"
#include "../include/sha256.h"
#include "../include/handle_endian.h"

void sha256_compute(u32 state[8], u8 *block_data) {
    static u32 K[64] = SHA256_K;
	
	u32 		w[64] = {0};				/* Message schedule (word) */
    u32 		a, b, c, d, e, f, g, h;		/* Register working variable */
    u32 		tmp1, tmp2;					/* Temporary variable */
    s32 		t;							/* Loop counter */

    /* Save register */
    a = state[0]; b = state[1]; c = state[2];
    d = state[3]; e = state[4]; f = state[5];
    g = state[6]; h = state[7];

	/* Fill the 16 first word value (message schedule) */
	block_to_u32(block_data, w);
	for (t = 0 ; t < 16; t++) {
		w[t] = SWAP_BYTE_32(w[t]);
	}
    for (t = 16; t < 64; ++t) {
        w[t] = SSIG1(w[t - 2]) + w[t - 7] + SSIG0(w[t - 15]) + w[t - 16];
    }

    /* Perform the main hash computation */
    for (t = 0; t < 64; ++t) {
        tmp1 = h + BSIG1(e) + CH(e, f, g) + K[t] + w[t];
        tmp2 = BSIG0(a) + MAJ(a, b, c);
        /* Swap register value */
		h = g; g = f; f = e;
        e = d + tmp1;
        d = c; c = b; b = a;
        a = tmp1 + tmp2;
    }

    /* Add the working variables back into the current hash value */
    state[0] += a; state[1] += b; state[2] += c;
    state[3] += d; state[4] += e; state[5] += f;
    state[6] += g; state[7] += h;
}


t_list *SHA256_init(u32 state[8], u8 *str, u64 len) {
	state[0] = SHA256_R0; state[1] = SHA256_R1;
	state[2] = SHA256_R2; state[3] = SHA256_R3;
	state[4] = SHA256_R4; state[5] = SHA256_R5;
	state[6] = SHA256_R6; state[7] = SHA256_R7;
	return (build_block_list(str, len, TRUE, BYTES_LAST_BLOCK_SIZE));
}

void SHA256_fill_hash(u32 *hash, u32 state[8]) {
	for (u32 i = 0; i < 8; i++) {
		hash[i] = state[i];
	}
}

void SHA256_hash(HashCtx *ctx, u8 *str, u64 len) {
	t_list		*block_list = NULL, *current = NULL;
	u32			state[8] = {0}, i = 0, list_size = 0;

	if (!(block_list = SHA256_init(state, str, len))) {
		ft_printf_fd(2, "Error: SHA256_hash: SHA256_init failed\n");
		return;
	}
	list_size = ft_lstsize(block_list);
	current = block_list;
	while (i < list_size) {
		sha256_compute(state, current->content);
		i++;
		current = current->next;
	}
	SHA256_fill_hash(ctx->hash, state);
	ft_lstclear(&block_list, free);
}

void SHA256_set_context(HashCtx *ctx) {
	ctx->hash_size = (SHA256_DIGEST_SIZE >> 2);
	ctx->hash_func = SHA256_hash;
	ctx->algo_name = ft_strdup("SHA256");
}