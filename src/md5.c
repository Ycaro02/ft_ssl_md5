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

static inline u32 MD5_rounds_compute(u32 a, u32 func_compute, u32 k_add_m, u32 s) {
	return (ROTATE_LEFT_32((a + func_compute + k_add_m), s));
}

static void MD5_swap_compute(u32 buff[4], u32 compute) {
	u32 tmp = buff[3];

	buff[3] = buff[2];
	buff[2] = buff[1];
	buff[1] += compute;
	buff[0] = tmp;
}

void MD5_block_compute(u32 buff[4], u8 *block_data, u32 K[64]) {
	u32 M[16] = {0};
	u32 compute, m_idx, a, b, c, d;

	a = buff[0];
	b = buff[1];
	c = buff[2];
	d = buff[3];

	block_to_u32(block_data, M);

	for (s32 i = 0; i < MD5_IT_NB; i++) {
		if (i < 16) {
			m_idx = i;
			compute = MD5_rounds_compute(buff[0], func_f(buff[1], buff[2], buff[3]), (K[i] + M[m_idx]), MD5_SHIFT1[i % 4]);
		} else if (i < 32) {
			m_idx = (5 *i + 1) % 16; 
			compute = MD5_rounds_compute(buff[0], func_g(buff[1], buff[2], buff[3]), (K[i] + M[m_idx]),  MD5_SHIFT2[i % 4]);
		} else if (i < 48) {
			m_idx = (3 *i + 5) % 16;
			compute = MD5_rounds_compute(buff[0], func_h(buff[1], buff[2], buff[3]), (K[i] + M[m_idx]),  MD5_SHIFT3[i % 4]);
		} else {
			m_idx = (7 * i) % 16;
			compute = MD5_rounds_compute(buff[0], func_i(buff[1], buff[2], buff[3]), (K[i] + M[m_idx]),  MD5_SHIFT4[i % 4]);
		}
		MD5_swap_compute(buff, compute);
	}
	buff[0] += a;
	buff[1] += b;
	buff[2] += c;
	buff[3] += d;
}


void MD5_fill_hash(u32 *hash, u32 buff[4]) {
	hash[0] = SWAP_BYTE_32(buff[0]);
	hash[1] = SWAP_BYTE_32(buff[1]);
	hash[2] = SWAP_BYTE_32(buff[2]);
	hash[3] = SWAP_BYTE_32(buff[3]);
}

t_list *MD5_init(u32 K[64], u32 buff[4], u8 *str, u64 len) {
	MD5_K_get(K);
	buff[0] = RA_HEX;
	buff[1] = RB_HEX;
	buff[2] = RC_HEX;
	buff[3] = RD_HEX;
	return (build_block_list(str, len, FALSE, FALSE));
}

/**
 * @brief Process the MD5 algorithm
 * @param input input string to hash
 */
void MD5_hash(HashCtx *ctx, u8 *str, u64 len) {
	t_list		*block_list = NULL, *current = NULL;
	u32			i = 0;
	u32			K_constant[MD5_IT_NB] = {0}, buffer[4] = {0},  lst_size = 0;

	if (!(block_list = MD5_init(K_constant, buffer, str, len))) {
		ft_printf_fd(2, "Error: MD5_hash: MD5_init failed\n");
		return;
	}

	lst_size = ft_lstsize(block_list);
	current = block_list;

	while (i < lst_size) {
		MD5_block_compute(buffer, current->content, K_constant);
		i++;
		current = current->next;
	}

	MD5_fill_hash(ctx->hash, buffer);
	ft_lstclear(&block_list, free);
}

void MD5_set_context(HashCtx *ctx) {
	ctx->hash_func = MD5_hash;
	ctx->algo_name = ft_strdup("MD5");
	/* (MD5_DIGEST_SIZE / 4) */
	ctx->hash_size = (MD5_DIGEST_SIZE >> 2);
}

