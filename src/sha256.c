#include "../include/ft_ssl.h"
#include "../include/sha256.h"
#include "../include/handle_endian.h"

void sha256_compute(SHA256_Ctx *ctx, u8 *block_data) {
    static u32 K[64] = SHA256_K;
	
	u32 		w[64] = {0};				/* Message schedule (word) */
    u32 		a, b, c, d, e, f, g, h;		/* Register working variable */
    u32 		tmp1, tmp2;					/* Temporary variable */
    s32 		t;							/* Loop counter */

    /* Save register */
    a = ctx->state[0]; b = ctx->state[1]; c = ctx->state[2];
    d = ctx->state[3]; e = ctx->state[4]; f = ctx->state[5];
    g = ctx->state[6]; h = ctx->state[7];

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
    ctx->state[0] += a; ctx->state[1] += b; ctx->state[2] += c;
    ctx->state[3] += d; ctx->state[4] += e; ctx->state[5] += f;
    ctx->state[6] += g; ctx->state[7] += h;
}


void SHA256_init(SHA256_Ctx *c, u8 *str, u64 len) {
	c->state[0] = SHA256_R0; c->state[1] = SHA256_R1;
	c->state[2] = SHA256_R2; c->state[3] = SHA256_R3;
	c->state[4] = SHA256_R4; c->state[5] = SHA256_R5;
	c->state[6] = SHA256_R6; c->state[7] = SHA256_R7;
	c->input = str;
	c->input_size = len;
	c->block_list = build_block_list(str, len, TRUE);
	c->list_size = ft_lstsize(c->block_list);
}

void SHA256_fill_hash(u32 *hash, SHA256_Ctx *c) {
	for (u32 i = 0; i < 8; i++) {
		hash[i] = c->state[i];
	}

}

void SHA256_hash_str(HashCtx *ctx, u8 *str, u64 len) {
	SHA256_Ctx	c = {0};
	t_list		*current = NULL;
	u32			i = 0;

	SHA256_init(&c, str, len);
	current = c.block_list;
	while (i < c.list_size) {
		sha256_compute(&c, current->content);
		i++;
		current = current->next;
	}
	SHA256_fill_hash(ctx->hash, &c);
}

s8 SHA256_hash_file(HashCtx *ctx, char *path) {
	char	*file_content = NULL;
	u64		file_size = 0;

	if (!(file_content = sstring_read_fd(-1, path, &file_size))) {
		return (FALSE);
	}
	SHA256_hash_str(ctx, (u8 *)file_content, file_size);
	free(file_content);
	return (TRUE);
}

void SHA256_Ctx_free(SHA256_Ctx *c) {
	ft_lstclear(&c->block_list, free);
}

void SHA256_set_context(HashCtx *ctx) {
	ctx->hash_size = (SHA256_DIGEST_SIZE >> 2);
	ctx->hash_str_func = SHA256_hash_str;
	ctx->hash_file_func = SHA256_hash_file;
	ctx->algo_name = ft_strdup("SHA256");
}