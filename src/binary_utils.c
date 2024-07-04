#include "../include/ft_ssl.h"
#include "../include/handle_endian.h"

char *u64_to_binary(u64 n) {
	char	*binary = NULL;
	s32		i = 63;

	if (!(binary = malloc(64 + 1))) {
		return (NULL);
	}
	binary[64] = '\0';
	while (i >= 0) {
		binary[i] = n % 2 + '0';
		n /= 2;
		i--;
	}
	return (binary);
}

/**
 * @brief Convert a char to a binary string
 * @param c char to convert
 * @return allocated char * c converted to binary
*/
char *char_to_binary(char c)
{
	char	*binary = NULL;
	s32		i = 7;

	if (!(binary = malloc(8 + 1))) {
		return (NULL);
	}
	binary[8] = '\0';
	while (i >= 0) {
		binary[i] = c % 2 + '0';
		c /= 2;
		i--;
	}
	return (binary);
}

/**
 * @brief Convert a string to a binary string
 * @param str string to convert
 * @return allocated char * str converted to binary
*/
char *string_to_binary(char *str) {
	char	*binary = NULL;
	s32		i = 0, j = 0;

	if (!(binary = malloc(8 * ft_strlen(str) + 1))) {
		ft_printf_fd(2, "Error: string_to_binary: malloc failed\n");
		return (NULL);
	}
	while (str[i]) {
		char *tmp = char_to_binary(str[i]);
		if (!tmp) {
			ft_printf_fd(2, "Error: string_to_binary: malloc failed\n");
			free(binary);
			return (NULL);
		} else if (ft_strlen(tmp) != 8) {
			ft_printf_fd(2, "Error: string_to_binary: tmp is not 8 bytes long, |%s| len: %d\n", tmp, ft_strlen(tmp));
			free(binary);
			free(tmp);
			return (NULL);
		}
		ftlib_strcpy(binary + j, tmp, 8);
		j += 8;
		free(tmp);
		i++;
	}
	binary[j] = '\0';
	return (binary);
}

/**
 * @brief Get a padding string
 * @param size size of the padding
 * @param need_padding_1 if we need to add a 1 at the beginning
 * @return allocated char * padding string
*/
char *get_padding_str(u32 size, s8 need_padding_1) {
	char *block = NULL;

	if (!(block = malloc(size + 1))) {
		ft_printf_fd(2, "Error: get_padding_str: malloc failed\n");
		return (NULL);
	}
	ft_memset(block, '0', size);
	block[size] = '\0';
	if (need_padding_1) {
		block[0] = '1';
	}
	return (block);
}


/**
 * @brief Build a binary block with padding if needed, and add the length of the string if it's the last block
 * @param str string to check
 * @param base_len length of the string
 * @param last_block if it's the last block
 * @return allocated char *str with padding if needed
*/
char *build_binary_block(char *input_string, u64 base_len, s8 last_block, s8 need_padding_1) {
	
	/* If it's the last block adapt block_size  */
	u32		block_size = last_block ? (MD5_LAST_BLOCK_SIZE + 1U) : MD5_BLOCK_SIZE; 
	u32		mod = ft_strlen(input_string) % block_size;
	u32		to_add = mod == 0 ? 0 : block_size - mod;
	char	*block_str = NULL, *padding = NULL, *len_str = NULL;

	if (mod != 0 || last_block) {
		if (!(padding = get_padding_str(to_add, need_padding_1))) {
			ft_printf_fd(2, "Error: build_binary_block: get_padding_str failed\n");
			return (NULL);
		}
		block_str = ft_strjoin_free(input_string, padding, 's');
	} else {
		block_str = ft_strdup(input_string);
	}

	if (last_block) {
		if (!(len_str = u64_to_binary(base_len))) {
			ft_printf_fd(2, "Error: build_binary_block: u64_to_binary failed\n");
			return (NULL);
		}
		block_str = ft_strjoin_free(block_str, len_str, 'a');
	}
	// ft_printf_fd(1, "Final str: %s\nlen %d\n", block_str, ft_strlen(block_str));
	return (block_str);
}

/**
 * @brief Convert a string to a binary string block list of 512 bits
 * @param str string to convert (in binary format)
 * @return t_list * list of binary block string
*/
t_list *string_to_binary_block_list(char *str) {
	t_list	*binary_list = NULL, *block = NULL;
	char	*binary_str = NULL, *padding = NULL;
	u64		base_len = ft_strlen(str), tmp_len = 0;
	u32		i = 0;
	char	save_char = 0;

	/* Bool to handle block */
	s8		is_last_block = FALSE, next_is_last = FALSE, need_padding_1 = FALSE, trunc_needed = FALSE;

	while (!is_last_block) {
		tmp_len = base_len - i;
		is_last_block = tmp_len <= (MD5_LAST_BLOCK_SIZE) ? TRUE : FALSE;


		/* Check if we are in special case and adapt bool logic */
		if (next_is_last) {
			is_last_block = TRUE;
		} else if (tmp_len > MD5_LAST_BLOCK_SIZE && tmp_len <= MD5_BLOCK_SIZE) {
			// ft_printf_fd(1, RED"Block not full need ADAPT: \n"RESET);
			next_is_last = TRUE;
			if (tmp_len == MD5_BLOCK_SIZE) {
				need_padding_1 = TRUE;
			}
		}

		trunc_needed = (!is_last_block && !next_is_last);
		/* Trunc string if we are not the last block and we have enought data (not next is last) */
		if (trunc_needed) {
			save_char = str[i + MD5_BLOCK_SIZE];
			str[i + MD5_BLOCK_SIZE] = '\0';
		}

		/* If we are the last block and we need special padding (next is last == true) */
		if (is_last_block && next_is_last) {
			padding = get_padding_str(MD5_LAST_BLOCK_SIZE, need_padding_1);
			binary_str = build_binary_block(padding, base_len, is_last_block, FALSE);
			free(padding);
		} else { /* Classic build */
			binary_str = build_binary_block(str + i, base_len, is_last_block, TRUE);
			if (trunc_needed) {
				str[i + MD5_BLOCK_SIZE] = save_char;
			}
		}

		block = ft_lstnew(binary_str);
		ft_lstadd_back(&binary_list, block);
		i += MD5_BLOCK_SIZE;
	}
	return (binary_list);
}

/**
 * @brief Convert a binary string to a u32
 * @param binary binary string
 * @param size size to read in the binary string
 * @return u32 converted binary string
 */
u32 binary_string_to_u32(char *binary, u32 size) {
	u32 i = 0;
	u32 res = 0;

	while (i < size) {
		res = res * 2 + (binary[i] - '0');
		i++;
	}
	return (res);
}

/**
 * @brief Split a block into 16 words of 32 bits
 * @param block block to split
 * @param splited_block splited block to fill
 * @param block_idx block index
 */
void split_block_into_word(char *block, u32 **splited_block, u32 block_idx) {
	u32 i = 0;
	u32 j = 0;

	while (j < 16) {
		splited_block[block_idx][j] = binary_string_to_u32(block + i, 32);
		i += 32;
		j++;
	}

	ft_printf_fd(1, "Splited block:\n");
	for (u32 i = 0; i < 16; i++) {
		ft_printf_fd(1, "Data %d M%d: 0x%X -> %u\n", block_idx, i, splited_block[block_idx][i], splited_block[block_idx][i]);
	}
}

#include <math.h>

/**
 * @brief Compute K constant
 * @param K K constant to fill
 */
void compute_K_constant(u32 K[64]) {
	s32 i = 0;

	while (i < 64) {
		K[i] = (u32)(fabs(sin(i + 1)) * (1UL << 32U));
		i++;
	}
}

/**
 * @brief Initialize the MD5 context
 * @param c MD5 context to initialize
 * @param input input string to hash (in ascii)
 */
void MD5_init(MD5_Context *c, char *input) {
	u32 i = 0;

	c->input = input;
	c->binary_input = string_to_binary(input);
	c->block_list = string_to_binary_block_list(c->binary_input);
	
	c->input_size = ft_strlen(input);
	c->binary_input_size = c->input_size * 8;
	c->list_size = ft_lstsize(c->block_list);
	c->splited_block = malloc(sizeof(u32 *) * c->list_size);

	while (i < c->list_size) {
		c->splited_block[i] = malloc(sizeof(u32) * 16);
		i++;
	}

	c->A = RA_HEX;
	c->B = RB_HEX;
	c->C = RC_HEX;
	c->D = RD_HEX;

	compute_K_constant(c->K);
	// for (u32 i = 0; i < 64; i++) {
	// 	ft_printf_fd(1, "K[%d]: 0x%X\n", i, c->K[i]);
	// }
}

void MD5_context_free(MD5_Context *c) {
	ft_lstclear(&c->block_list, free);
	for (u32 i = 0; i < c->list_size; i++) {
		free(c->splited_block[i]);
	}
	free(c->splited_block);
	free(c->binary_input);
}



/**
 * @brief MD5 rounds routine
 * @param c MD5 context
 * @param S shift array
 * @param func function ptr
 * @param n round number
 */
// void MD5_rounds(MD5_Context *c, s32 S[4], u32 *M, u32 (*func)(u32, u32, u32), u32 n, s32 m_idx) {
//     u32 DD = c->D;
// 	u32 CC = c->C;
// 	u32 BB = c->B;

// 	if (n < 16)
// 		ft_printf_fd(1, "--------------------------------\nBefore A: 0x%X -> B: 0x%X -> C: 0x%X -> D: 0x%X\n", c->A, c->B, c->C, c->D);
//     c->D = c->C;
//     c->C = c->B;
//     // c->B = c->B + left_rotate((c->A + func(c->B, CC, DD) + M[m_idx] + c->K[n]), S[n % 4]);
// 	c->B = BB + left_rotate((c->A + func(BB, CC, DD) + M[m_idx] + c->K[n]), S[n%4]);
//     c->A = DD;


// 	if (n < 16) {
// 		ft_printf_fd(1, "For round %u: we use M[%d] == 0x%x -> S:%d, K:%X\n", n + 1, m_idx, M[m_idx], S[n % 4], c->K[n]);
// 		ft_printf_fd(1, "After A: 0x%X -> B: 0x%X -> C: 0x%X -> D: 0x%X\n", c->A, c->B, c->C, c->D);
// 	}
// }
u32 left_rotate(u32 x, u32 c) {
	return ((x << c) | (x >> (32 - c)));
}

#define ROTATE_LEFT(val, shift)	(((val << shift) | (val >> (32 - shift))))

u32	func_f(u32 B, u32 C, u32 D) { return ((B & C) | ((~B) & D)); }
u32	func_g(u32 B, u32 C, u32 D) { return ((B & D) | (C & (~D))); }
u32	func_h(u32 B, u32 C, u32 D) { return (B ^ C ^ D); }
u32	func_i(u32 B, u32 C, u32 D) { return (C ^ (B | (~D))); }

// u32 MD5_rounds_compute(MD5_Context *c, s32 S[4], u32 *M, u32 (*func)(u32, u32, u32), u32 n, s32 m_idx) {

// 	u32 compute = func(c->B, c->C, c->D);
// 	compute += c->A;
// 	compute += M[m_idx];
// 	compute += c->K[n];
// 	return (left_rotate(compute, S[n % 4]));
// }


void MD5_block_compute(MD5_Context *c, u32 block_idx) {
	int i = 0;
	u32 tmp, compute;
	// static s32 select_m[64] = M_SELECTION_ORDER;
    static s32 select_m[] = {
        // Round 1
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        // Round 2
        1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
        // Round 3
        5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
        // Round 4
        0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9
    };

	static s32 shift_val[] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	// static s32 MD5_SHIFT1[4] = { 7, 12, 17, 22 };
	// static s32 MD5_SHIFT2[4] = { 5, 9, 14, 20 };
	// static s32 MD5_SHIFT3[4] = { 4, 11, 16, 23 };
	// static s32 MD5_SHIFT4[4] = { 6, 10, 15, 21 };

	(void)block_idx;
	u32 *M = c->splited_block[0];

	while (i < 64) {
		if (i < 16) {
			compute = func_f(c->B, c->C, c->D);
			// compute = MD5_rounds_compute(c, MD5_SHIFT1, c->splited_block[block_idx], func_f, i, select_m[i]);
		} else if (i < 32) {
			compute = func_g(c->B, c->C, c->D);
			// compute = MD5_rounds_compute(c, MD5_SHIFT2, c->splited_block[block_idx], func_g, i, select_m[i]);
		} else if (i < 48) {
			compute = func_h(c->B, c->C, c->D);
			// compute = MD5_rounds_compute(c, MD5_SHIFT3, c->splited_block[block_idx], func_h, i, select_m[i]);
		} else {
			compute = func_i(c->B, c->C, c->D);
			// compute = MD5_rounds_compute(c, MD5_SHIFT4, c->splited_block[block_idx], func_i, i, select_m[i]);
		}
		tmp = c->D;
		c->D = c->C;
		c->C = c->B;
		// c->B = (c->B + compute) % (1UL << 32);
		c->B = c->B + ROTATE_LEFT((c->A + compute + c->K[i] + M[select_m[i]]), shift_val[i]);
		c->A = tmp;
		i++;
	}
	c->A = (c->A + RA_HEX) % (1UL << 32);
	c->B = (c->B + RB_HEX) % (1UL << 32);
	c->C = (c->C + RC_HEX) % (1UL << 32);
	c->D = (c->D + RD_HEX) % (1UL << 32);
}

#include <stdio.h>

void display_hash(MD5_Context *c) {
	printf("Brut display %08x%08x%08x%08x\n", c->A, c->B, c->C, c->D);
	printf("Swap endian  %08x%08x%08x%08x\n", SWAP_BYTE_32(c->A), SWAP_BYTE_32(c->B), SWAP_BYTE_32(c->C), SWAP_BYTE_32(c->D));
}

/**
 * @brief Process the MD5 algorithm
 * @param input input string to hash
 * TODO: Implement the MD5 algorithm
 * We need to parse input before this func to give size, particulary for the file case
 * they can contain \0 and we need to know the size of the file (data to read)
 */
void MD5_process(char *input) {
	MD5_Context c = {0};
	t_list		*current = NULL;
	u32			i = 0;

	MD5_init(&c, input);
	ft_printf_fd(1, CYAN"Input: %s\nInput Bin %s\n", input, c.binary_input);
	ft_printf_fd(1, "Input size: %u --> %u == 0x%x\n", c.input_size, c.binary_input_size, c.binary_input_size);
	ft_printf_fd(1, "List size: %u\n"RESET, c.list_size);
	current = c.block_list;
	while (i < c.list_size) {
		ft_printf_fd(1, ORANGE"Block content:\n"RESET"%s\n", current->content);
		split_block_into_word(current->content, c.splited_block, i);
		i++;
		current = current->next;
	}

	MD5_block_compute(&c, 0);

	display_hash(&c);

	MD5_context_free(&c);
}