#include "../include/ft_ssl.h"
#include "../include/handle_endian.h"

/**
 * @brief Convert a string to a binary string
 * @param str string to convert
 * @return allocated char * str converted to binary
*/
char *string_to_binary(u8 *str, u64 len) {
	char	*binary = NULL, *tmp = NULL;
	u64		i = 0, j = 0;

	if (!(binary = malloc(8 * len + 1U))) {
		ft_printf_fd(2, "Error: string_to_binary: malloc failed\n");
		return (NULL);
	}
	while (i < len) {
		// tmp = char_to_binary(str[i]);
		DIGIT_TO_BINSTR(str[i], 8, tmp);
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
		j += 8U;
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
char *build_binary_block(char *input_string, u64 base_len, u32 block_size, u32 last_block_size, s8 last_block, s8 need_padding_1) {
	
	/* If it's the last block adapt block_size  */
	u32		size = last_block ? (last_block_size + 1U) : block_size; 
	u32		mod = ft_strlen(input_string) % size;
	u32		to_add = mod == 0 ? 0 : size - mod;
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
		// if (!(len_str = u64_to_binary(base_len))) {
		DIGIT_TO_BINSTR(base_len, 64, len_str);
		if (!len_str) {
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
t_list *binary_string_to_block_lst(char *str, u32 block_size, u32 last_block_size) {
	t_list	*binary_list = NULL, *block = NULL;
	char	*binary_str = NULL, *padding = NULL;
	u64		base_len = ft_strlen(str), tmp_len = 0;
	u64		i = 0;
	char	save_char = 0;

	/* Bool to handle block */
	s8		is_last_block = FALSE, next_is_last = FALSE, need_padding_1 = FALSE, trunc_needed = FALSE;

	while (!is_last_block) {
		tmp_len = base_len - i;
		is_last_block = tmp_len <= (last_block_size) ? TRUE : FALSE;

		/* Check if we are in special case and adapt bool logic */
		if (next_is_last) {
			is_last_block = TRUE;
		} else if (tmp_len > last_block_size && tmp_len <= block_size) {
			// ft_printf_fd(1, RED"Block not full need ADAPT: \n"RESET);
			next_is_last = TRUE;
			if (tmp_len == block_size) { need_padding_1 = TRUE; }
		}

		trunc_needed = (!is_last_block && !next_is_last);
		/* Trunc string if we are not the last block and we have enought data (not next is last) */
		if (trunc_needed) {
			save_char = str[i + block_size];
			str[i + block_size] = '\0';
		}

		/* If we are the last block and we need special padding (next is last == true) */
		if (is_last_block && next_is_last) {
			padding = get_padding_str(last_block_size, need_padding_1);
			binary_str = build_binary_block(padding, base_len, block_size, last_block_size, is_last_block, FALSE);
			free(padding);
		} else { /* Classic build */
			binary_str = build_binary_block(str + i, base_len, block_size, last_block_size, is_last_block, TRUE);
			if (trunc_needed) { str[i + block_size] = save_char; }
		}

		block = ft_lstnew(binary_str);
		ft_lstadd_back(&binary_list, block);
		i += block_size;
	}
	return (binary_list);
}

/**
 * @brief Convert a binary string to a u32
 * @param binary binary string
 * @param size size to read in the binary string
 * @return u32 converted binary string (in big endian)
 */
u32 binary_string_to_u32(char *binary, u32 size, s8 rev_endian) {
	u32 i = 0;
	u32 res = 0;

	while (i < size) {
		// res = res * 2 + (binary[i] - '0');
		res = (res << 1) + (binary[i] - '0');
		i++;
	}
	if (rev_endian) {
		return (SWAP_BYTE_32(res));
	}
	return (res);
}
