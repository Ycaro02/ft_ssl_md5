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
		splited_block[block_idx][j] = SWAP_BYTE_32(binary_string_to_u32(block + i, 32));
		i += 32;
		j++;
		// if (j == 16) {
		// 	u32 tmp = splited_block[block_idx][15];
		// 	splited_block[block_idx][15] = splited_block[block_idx][14];
		// 	splited_block[block_idx][14] = tmp;
		// }
	}


	ft_printf_fd(1, "Splited block:\n");
	for (u32 i = 0; i < 16; i++) {
		ft_printf_fd(1, "Data %d M%d: 0x%X -> %u\n", block_idx, i, splited_block[block_idx][i], splited_block[block_idx][i]);
	}
}

#include <stdio.h>

void display_hash(MD5_Context *c) {
	printf("Brut display %08x%08x%08x%08x\n", c->A, c->B, c->C, c->D);
	printf("Swap endian  %08x%08x%08x%08x\n", SWAP_BYTE_32(c->A), SWAP_BYTE_32(c->B), SWAP_BYTE_32(c->C), SWAP_BYTE_32(c->D));
}

#include <math.h>

/**
 * @brief Compute K constant
 * @param K K constant to fill
 */
void compute_K_constant(u32 K[64]) {
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
	for (u32 i = 0; i < 64; i++) {
		ft_printf_fd(1, "K[%d]: 0x%X\n", i + 1, c->K[i]);
	}
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
u32	func_g(u32 B, u32 C, u32 D) { return ((D & B) | ((~D) & C)); }
u32	func_h(u32 B, u32 C, u32 D) { return (B ^ C ^ D); }
u32	func_i(u32 B, u32 C, u32 D) { return (C ^ (B | (~D))); }

// u32 MD5_rounds_compute(MD5_Context *c, s32 S[4], u32 *M, u32 (*func)(u32, u32, u32), u32 n, s32 m_idx) {

// 	u32 compute = func(c->B, c->C, c->D);
// 	compute += c->A;
// 	compute += M[m_idx];
// 	compute += c->K[n];
// 	return (left_rotate(compute, S[n % 4]));
// }

void MD5_block_compute(MD5_Context *con, u32 block_idx) {
	u32 tmp, compute;

	s32 g = 0;

	static s32 shift_val[] = {
		7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

	static const u32 k[] = {
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

	(void)block_idx;
	u32 *M = con->splited_block[0];

	u32 a,b,c,d;

	a = con->A;
	b = con->B;
	c = con->C;
	d = con->D;

	printf("a=%x, b=%x, c=%x, d=%x\n", a,b,c,d);

	for (s32 i = 0; i < 64; i++) {
		if (i < 16) {
			compute = func_f(b, c, d);
			g = i;
			// compute = MD5_rounds_compute(c, MD5_SHIFT1, con->splited_block[block_idx], func_f, i, select_m[i]);
		} else if (i < 32) {
			compute = func_g(b, c, d);
			g = (5*i + 1) % 16; 
			// compute = MD5_rounds_compute(c, MD5_SHIFT2, con->splited_block[block_idx], func_g, i, select_m[i]);
		} else if (i < 48) {
			compute = func_h(b, c, d);
			g = (3*i + 5) % 16;
			// compute = MD5_rounds_compute(c, MD5_SHIFT3, con->splited_block[block_idx], func_h, i, select_m[i]);
		} else {
			compute = func_i(b, c, d);
			g = (7*i) % 16;
			// compute = MD5_rounds_compute(c, MD5_SHIFT4, con->splited_block[block_idx], func_i, i, select_m[i]);
		}
		tmp = d;
		d = c;
		c = b;
		b = b + ROTATE_LEFT((a + compute + k[i] + M[g]), shift_val[i]);
		a = tmp;
		if (i < 17)
			printf("%d: a=%x, b=%x, c=%x, d=%x\n",i,a,b,c,d);
	}
	display_hash(con);
	con->A += a;
	con->B += b;
	con->C += c;
	con->D += d;

	// c->A = (c->A + RA_HEX) % (1UL << 32);
	// c->B = (c->B + RB_HEX) % (1UL << 32);
	// c->C = (c->C + RC_HEX) % (1UL << 32);
	// c->D = (c->D + RD_HEX) % (1UL << 32);
	display_hash(con);


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

	// display_hash(&c);

	MD5_context_free(&c);
}