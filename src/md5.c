#include <math.h>
#include "../include/ft_ssl.h"
#include "../include/md5.h"
#include "../include/handle_endian.h"

/**
 * @brief Compute K constant
 * @param K K constant to fill
 */
void MD5_K_get(u32 K[MD5_IT_NB]) {
	s32 i = 0;

	while (i < MD5_IT_NB) {
		K[i] = floor(fabs(sin(i + 1)) * (1UL << 32U));
		i++;
	}
}

/**
 * @brief Initialize the MD5 context
 * @param c MD5 context to initialize
 * @param input input string to hash (in ascii)
 */
void MD5_init(MD5_Ctx *c, u8 *input, u64 input_size) {
	t_list *tmp = NULL;

	c->input = input;
	c->input_size = input_size;
	MD5_K_get(c->K);
	if (!(c->block_list = build_block_list(c->input, c->input_size))) {
		ft_printf_fd(2, "Error: MD5_init: build_block_list failed\n");
		return;
	}
	c->list_size = ft_lstsize(c->block_list);
	if (!(c->splited_block = malloc(sizeof(u32 *) * c->list_size))) {
		ft_printf_fd(2, "Error: MD5_init: malloc failed\n");
		return;
	}

	tmp = c->block_list;
	for (u32 i = 0; i < c->list_size; i++) {
		if (!(c->splited_block[i] = malloc(sizeof(u32) * MD5_NB_WORD))) {
			ft_printf_fd(2, "Error: MD5_init: malloc failed\n");
			free_incomplete_array((void **)c->splited_block, i);
			return ;
		}
		block_to_u32(tmp->content, c->splited_block[i]);
		tmp = tmp->next;
	}
	c->A = RA_HEX;
	c->B = RB_HEX;
	c->C = RC_HEX;
	c->D = RD_HEX;
}

void MD5_Ctx_free(MD5_Ctx *c) {
	ft_lstclear(&c->block_list, free);
	for (u32 i = 0; i < c->list_size; i++) {
		free(c->splited_block[i]);
	}
	free(c->splited_block);
}



static inline u32 MD5_rounds_compute(u32 a, u32 func_compute, u32 k_add_m, u32 s) {
	return (ROTATE_LEFT((a + func_compute + k_add_m), s));
}

static void MD5_swap_compute(MD5_Ctx *ctx, u32 compute) {
	u32 tmp = ctx->D;

	ctx->D = ctx->C;
	ctx->C = ctx->B;
	ctx->B += compute;
	ctx->A = tmp;

}

void MD5_block_compute(MD5_Ctx *ctx, u32 block_idx) {
	u32 *M = ctx->splited_block[block_idx];
	u32 compute, m_idx, a, b, c, d;

	a = ctx->A;
	b = ctx->B;
	c = ctx->C;
	d = ctx->D;


	for (s32 i = 0; i < MD5_IT_NB; i++) {
		if (i < 16) {
			m_idx = i;
			compute = MD5_rounds_compute(ctx->A, func_f(ctx->B, ctx->C, ctx->D), (ctx->K[i] + M[m_idx]), MD5_SHIFT1[i % 4]);
		} else if (i < 32) {
			m_idx = (5 *i + 1) % 16; 
			compute = MD5_rounds_compute(ctx->A, func_g(ctx->B, ctx->C, ctx->D), (ctx->K[i] + M[m_idx]),  MD5_SHIFT2[i % 4]);
		} else if (i < 48) {
			m_idx = (3 *i + 5) % 16;
			compute = MD5_rounds_compute(ctx->A, func_h(ctx->B, ctx->C, ctx->D), (ctx->K[i] + M[m_idx]),  MD5_SHIFT3[i % 4]);
		} else {
			m_idx = (7 * i) % 16;
			compute = MD5_rounds_compute(ctx->A, func_i(ctx->B, ctx->C, ctx->D), (ctx->K[i] + M[m_idx]),  MD5_SHIFT4[i % 4]);
		}
		MD5_swap_compute(ctx, compute);
	}
	ctx->A += a;
	ctx->B += b;
	ctx->C += c;
	ctx->D += d;
}


void MD5_fill_hash(u32 *hash, MD5_Ctx *c) {
	hash[0] = SWAP_BYTE_32(c->A);
	hash[1] = SWAP_BYTE_32(c->B);
	hash[2] = SWAP_BYTE_32(c->C);
	hash[3] = SWAP_BYTE_32(c->D);
}

/**
 * @brief Process the MD5 algorithm
 * @param input input string to hash
 */
void MD5_hash_str(HashCtx *ctx, u8 *str, u64 len) {
	MD5_Ctx		c = {0};
	t_list		*current = NULL;
	u32			i = 0;

	MD5_init(&c, str, len);
	current = c.block_list;
	while (i < c.list_size) {
		MD5_block_compute(&c, i);
		i++;
		current = current->next;
	}

	MD5_fill_hash(ctx->hash, &c);
	MD5_Ctx_free(&c);
}

/**
 * @brief Hash a file with MD5 algorithm
 * @param path file path
*/
s8 MD5_hash_file(HashCtx *ctx, char *path) {
	u64		file_size = 0;
	char	*file_map = sstring_read_fd(-1, path, &file_size);

	if (!file_map) {
		ft_printf_fd(1, "ft_ssl: md5: %s: No such file or directory\n", path);
		return (FALSE);
	}
	// ft_printf_fd(1, PINK"sstring_read_fd load File %s size: %u\n"RESET, path, file_size);
	MD5_hash_str(ctx, (u8 *)file_map, file_size);
	free(file_map);
	return (TRUE);
}


void MD5_set_context(HashCtx *ctx) {
	ctx->hash_file_func = MD5_hash_file;
	ctx->hash_str_func = MD5_hash_str;
	ctx->algo_name = ft_strdup("MD5");
	/* (MD5_DIGEST_SIZE / 4) */
	ctx->hash_size = (MD5_DIGEST_SIZE >> 2);
}

