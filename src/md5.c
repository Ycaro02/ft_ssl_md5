#include <math.h>
#include "../include/ft_ssl.h"
#include "../include/md5.h"
#include "../include/handle_endian.h"

/**
 * @brief Split a block into 16 words of 32 bits
 * @param block block to split
 * @param splited_block splited block to fill
 * @param block_idx block index
 */
void MD5_split_block(char *block, u32 **splited_block, u32 block_idx, u32 max_block) {
	u32 i = 0, j = 0, last_word = MD5_NB_WORD - 1;

	while (j < MD5_NB_WORD) {
		/* Convert binary string in u32 and inverse endian ( little to big )*/
		splited_block[block_idx][j] = binary_string_to_u32(block + i, 32, TRUE);
		i += 32;
		j++;
	}

	/* Handle special case for the len in the last block */
	if (block_idx == max_block -1) {
		u64 tmp = splited_block[block_idx][last_word] + splited_block[block_idx][last_word - 1];
		/* Restore len in little endian and update 2 last word in consequence */
		tmp = SWAP_BYTE_64(tmp);
		splited_block[block_idx][last_word - 1] = tmp >> 32;
		splited_block[block_idx][last_word] = tmp << 32;
	}

	// for (u32 i = 0; i < 16; i++) {
	// 	ft_printf_fd(1, "M%d = 0x%x\n", i, splited_block[block_idx][i]);
	// }
}




/**
 * @brief Compute K constant
 * @param K K constant to fill
 */
void MD5_K_get(u32 K[MD5_NB_ITERATION]) {
	s32 i = 0;

	while (i < MD5_NB_ITERATION) {
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
	u32 i = 0;

	c->input = input;

	/* TODO: Need to avoid this strlen call */
	c->input_size = input_size;
	c->binary_input_size = c->input_size * 8;
	
	
	c->binary_input = string_to_binary(input, c->input_size);
	c->block_list = binary_string_to_block_lst(c->binary_input, MD5_BLOCK_SIZE, MD5_LAST_BLOCK_SIZE);
	c->list_size = ft_lstsize(c->block_list);
	c->splited_block = malloc(sizeof(u32 *) * c->list_size);
	if (!c->splited_block) {
		ft_printf_fd(2, "Error: MD5_init: malloc failed\n");
		return;
	}

	MD5_K_get(c->K);

	while (i < c->list_size) {
		c->splited_block[i] = malloc(sizeof(u32) * MD5_NB_WORD);
		i++;
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
	free(c->binary_input);
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


	for (s32 i = 0; i < MD5_NB_ITERATION; i++) {
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

	MD5_fill_hash(ctx->hash, &c);
	// display_hash(ctx->hash, 4);

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
		ft_printf_fd(1, "ft_ssl: md5: %s No such file or directory\n", path);
		return (FALSE);
	}
	ft_printf_fd(1, PINK"sstring_read_fd load File %s size: %u\n"RESET, path, file_size);
	MD5_hash_str(ctx, (u8 *)file_map, file_size);
	free(file_map);
	return (TRUE);
}


void MD5_set_context(HashCtx *ctx) {
	ctx->hash_file_func = MD5_hash_file;
	ctx->hash_str_func = MD5_hash_str;
	ctx->hash_size = 4;
	ctx->algo_name = ft_strdup("MD5");
}

// To remove

/* Used for load file easier than read and multiple alloc */
// #include <sys/mman.h>
// #include <sys/stat.h>

// FT_INLINE u8 *mmap_file(char *path, u64 *file_size, u32 min_size)
// {
//     struct stat	st;
// 	void		*map;
//     int			fd = open(path, O_RDONLY);

//     if (fd == -1) {
//         ft_printf_fd(2, "Failed to open file %s\n", path);
//         return (NULL);
//     }

//     ft_bzero(&st, sizeof(st));
//     fstat(fd, &st);
//     *file_size = st.st_size;

//     if (*file_size <= min_size) {
//         close(fd);
//         ft_printf_fd(2, "File %s is empty\n", path);
//         return (NULL);
//     }

//     map = mmap(0, *file_size, PROT_READ, MAP_PRIVATE, fd, 0);

//     if (map == MAP_FAILED) {
//         close(fd);
//         ft_printf_fd(2, "Failed to open file %s\n", path);
//         return (NULL);
//     }
//     return (map);
// }

// u8		*map = mmap_file(path, &file_size, 0);
// u64 	map_file_size = 0;
// if (cmp_large_str((u8 *)file_map, map, map_file_size) == 0) {
// 	ft_printf_fd(1, GREEN"Same %s size: %u\n"RESET, path, file_size);
// } else {
// 	ft_printf_fd(1, RED"Not Same %s size: %u\n"RESET, path, file_size);
// }
// if (map) {
// 	ft_printf_fd(1, YELLOW"mmap load File %s size: %u\n"RESET, path, file_size);
// 	MD5_hash_str(map, file_size);
// 	munmap(map, map_file_size);
// }
// end to remove