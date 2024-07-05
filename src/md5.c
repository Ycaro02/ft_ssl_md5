#include <math.h>
#include "../include/ft_ssl.h"
#include "../include/handle_endian.h"

/**
 * @brief Split a block into 16 words of 32 bits
 * @param block block to split
 * @param splited_block splited block to fill
 * @param block_idx block index
 */
void MD5_split_block(char *block, u32 **splited_block, u32 block_idx, u32 max_block) {
	u32 i = 0;
	u32 j = 0;

	while (j < 16) {
		/* Convert binary string in u32 and inverse endian ( little to big )*/
		splited_block[block_idx][j] = binary_string_to_u32(block + i, 32, TRUE);
		i += 32;
		j++;
	}

	/* Handle special case for the len in the last block */
	if (block_idx == max_block -1) {
		u64 tmp = splited_block[block_idx][15] + splited_block[block_idx][14];
		/* Restore len in little endian and update 2 last word in consequence */
		tmp = SWAP_BYTE_64(tmp);
		splited_block[block_idx][14] = tmp >> 32;
		splited_block[block_idx][15] = tmp << 32;
	}

	// for (u32 i = 0; i < 16; i++) {
	// 	ft_printf_fd(1, "M%d = 0x%x\n", i, splited_block[block_idx][i]);
	// }
}


void display_hash(MD5_Context *c) {
	// printf("Brut display %08x%08x%08x%08x\n", c->A, c->B, c->C, c->D);
	// ft_printf_fd(1, YELLOW"Hash"RESET""PINK" (\"%s\") "RESET"\n", c->input);
	ft_printf_fd(1, ORANGE"%x%x%x%x\n"RESET, SWAP_BYTE_32(c->A), SWAP_BYTE_32(c->B), SWAP_BYTE_32(c->C), SWAP_BYTE_32(c->D));
}


/**
 * @brief Compute K constant
 * @param K K constant to fill
 */
void MD5_K_get(u32 K[64]) {
	s32 i = 0;

	while (i < 64) {
		K[i] = floor(fabs(sin(i + 1)) * (1UL << 32U));
		i++;
	}
}

/**
 * @brief Initialize the MD5 context
 * @param c MD5 context to initialize
 * @param input input string to hash (in ascii)
 */
void MD5_init(MD5_Context *c, u8 *input, u64 input_size) {
	u32 i = 0;

	c->input = input;

	/* TODO: Need to avoid this strlen call */
	c->input_size = input_size;
	c->binary_input_size = c->input_size * 8;
	
	
	c->binary_input = string_to_binary(input, c->input_size);
	c->block_list = binary_string_to_block_lst(c->binary_input);
	c->list_size = ft_lstsize(c->block_list);
	c->splited_block = malloc(sizeof(u32 *) * c->list_size);
	if (!c->splited_block) {
		ft_printf_fd(2, "Error: MD5_init: malloc failed\n");
		return;
	}

	MD5_K_get(c->K);

	while (i < c->list_size) {
		c->splited_block[i] = malloc(sizeof(u32) * 16);
		i++;
	}

	c->A = RA_HEX;
	c->B = RB_HEX;
	c->C = RC_HEX;
	c->D = RD_HEX;

}

void MD5_context_free(MD5_Context *c) {
	ft_lstclear(&c->block_list, free);
	for (u32 i = 0; i < c->list_size; i++) {
		free(c->splited_block[i]);
	}
	free(c->splited_block);
	free(c->binary_input);
}



static inline u32 MD5_rounds_compute(u32 a, u32 func_compute, u32 k_add_m, u32 s) {
	return (ROTATE_LEFT((a + func_compute + k_add_m), s));
}

static void MD5_swap_compute(MD5_Context *ctx, u32 compute) {
	u32 tmp = ctx->D;

	ctx->D = ctx->C;
	ctx->C = ctx->B;
	ctx->B += compute;
	ctx->A = tmp;

}

void MD5_block_compute(MD5_Context *ctx, u32 block_idx) {
	u32 *M = ctx->splited_block[block_idx];
	u32 compute, m_idx, a, b, c, d;

	a = ctx->A;
	b = ctx->B;
	c = ctx->C;
	d = ctx->D;

	// ft_printf_fd("a=%x, b=%x, c=%x, d=%x\n", a,b,c,d);

	for (s32 i = 0; i < 64; i++) {
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



/**
 * @brief Process the MD5 algorithm
 * @param input input string to hash
 * TODO: Implement the MD5 algorithm
 * We need to parse input before this func to give size, particulary for the file case
 * they can contain \0 and we need to know the size of the file (data to read)
 */
void MD5_process(u8 *input, u64 input_size) {
	MD5_Context c = {0};
	t_list		*current = NULL;
	u32			i = 0;

	MD5_init(&c, input, input_size);
	// ft_printf_fd(1, CYAN"Input: %s\nInput Bin %s\n", input, c.binary_input);
	// ft_printf_fd(1, "Input size: %u --> %u == 0x%x\n", c.input_size, c.binary_input_size, c.binary_input_size);
	// ft_printf_fd(1, "List size: %u\n"RESET, c.list_size);
	current = c.block_list;
	while (i < c.list_size) {
		// ft_printf_fd(1, ORANGE"Block content:\n"RESET"%s\n", current->content);
		MD5_split_block(current->content, c.splited_block, i, c.list_size);
		MD5_block_compute(&c, i);
		i++;
		current = current->next;
	}
	display_hash(&c);

	MD5_context_free(&c);
}

/**
 * @brief Hash a file with MD5 algorithm
 * @param path file path
*/
void MD5_hash_file(char *path) {
	u64 file_size = 0;
	u8 *file_map = mmap_file(path, &file_size, 0);
	if (file_map) {
		ft_printf_fd(1, YELLOW"File %s size: %u\n"RESET, path, file_size);
		MD5_process(file_map, file_size);
		munmap(file_map, file_size);
	}
}