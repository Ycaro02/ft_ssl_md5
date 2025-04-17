#include "../include/ft_ssl.h"
#include "../include/md5.h"

/**
 * @brief Pads the input data according to MD5 padding rules.
 * @param input The input data.
 * @param len The length of the input data.
 * @param new_len The length of the padded data.
 * @return The padded data.
 */
static u8 *input_padding(u8 *input, u64 len, u64 *new_len, s8 reverse_endian, s8 is_whirpool) {
    u64	bit_len = len * 8;
    u64	padding_len = 0;
	u8	*padded = NULL;
	
	/* if the length of the input data is less than 56 bytes, we need to add padding to the last block */
	if (is_whirpool) {
		// Get congruence of len mod 64
		if (len < 32) {
			padding_len = 32 - len;
		} else {
			padding_len = 64 + 32 - len;
		}
		// then just add 32 bytes for the length to complete the block
		*new_len = len + padding_len + 32;
	} else {
		if (len % BYTES_BLOCK_SIZE < BYTES_LAST_BLOCK_SIZE) {
			padding_len += BYTES_LAST_BLOCK_SIZE - (len % BYTES_BLOCK_SIZE);
		} else { /* else we need to add a new bloc for padding */
			padding_len += (BYTES_BLOCK_SIZE + BYTES_LAST_BLOCK_SIZE) - (len % BYTES_BLOCK_SIZE);
		}
		*new_len = len + padding_len + 8;
	}


	ft_printf_fd(2, "new_len: %u\n", *new_len);

    if (!(padded = malloc(*new_len))) {
        ft_printf_fd(2, "Error: input_padding: malloc failed\n");
        return (NULL);
    }

    ft_memcpy(padded, input, len);
    padded[len] = 0x80; /* Append a single '1' bit, 0b10000000 */
    ft_memset(padded + len + 1, 0, padding_len - 1);

    /* Append the original length in bits at the end of the padded data */
	if (reverse_endian) { /* if the endianess is reversed little -> big */
        for (int i = 0; i < 8; i++) {
            padded[*new_len - 8 + i] = (bit_len >> (56 - (i << 3))) & 0xFF;
        }
    } else {
        for (int i = 0; i < 8; i++) {
    		// padded[*new_len - 8 + i] = (bit_len >> (8 * i)) & 0xFF;
            padded[*new_len - 8 + i] = (bit_len >> (i << 3)) & 0xFF;
        }
    }

    return (padded);
}

/**
 * @brief Convert the padded data into a linked list of 512-bit blocks.
 * @param padded The padded data.
 * @param len The length of the padded data.
 * @return The head of the linked list of blocks.
 */
static t_list *data_to_block_list(u8 *padded, u64 len) {
    t_list *block_list = NULL;
    t_list *block = NULL;
	u8		*block_data = NULL;

    for (u64 i = 0; i < len; i += BYTES_BLOCK_SIZE) {
        if (!(block_data = malloc(BYTES_BLOCK_SIZE))) {
            ft_printf_fd(2, "Error: data_to_block_list: malloc failed\n");
			ft_lstclear(&block_list, free);
            return (NULL);
        }

        ft_memcpy(block_data, padded + i, BYTES_BLOCK_SIZE);
        if (!(block = ft_lstnew(block_data))) {
            free(block_data);
            ft_printf_fd(2, "Error: data_to_block_list: ft_lstnew failed\n");
			ft_lstclear(&block_list, free);
            return (NULL);
        }
        ft_lstadd_back(&block_list, block);
    }

    return (block_list);
}

/**
 * @brief Main function to prepare and process the data for the MD5 algorithm.
 * @param input The input data.
 * @param len The length of the input data.
 * @return The head of the linked list of 512-bit blocks.
 */
t_list *build_block_list(u8 *input, u64 len, s8 reverse_len_endian, s8 is_whirpool) {
    u64		new_len = 0;
    u8		*padded = NULL;
	t_list	*block_list = NULL;
	
    if (!(padded = input_padding(input, len, &new_len, reverse_len_endian, is_whirpool))) {
        return (NULL);
    }
    block_list = data_to_block_list(padded, new_len);
    free(padded);
    return (block_list);
}

/**
 * @brief Convert a portion of the padded data to a u32 array.
 * @param block The block to convert.
 * @return The u32 array.
 */
void block_to_u32(u8 *block, u32 *output) {
    for (int i = 0; i < MD5_NB_WORD; i++) {
        // output[i] = block[(i * 4)] | (block[(i * 4) + 1] << 8) | (block[(i * 4) + 2] << 16) | (block[(i * 4) + 3] << 24);
        output[i] = block[(i << 2)] | (block[(i << 2) + 1] << 8) | (block[(i << 2) + 2] << 16) | (block[(i << 2) + 3] << 24);
    }
}