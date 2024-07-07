#ifndef HEADER_FT_SSL_H
#define HEADER_FT_SSL_H

#include "../libft/libft.h"

/* binary_utils.c */
char	*string_to_binary(u8 *str, u64 len);
u32		binary_string_to_u32(char *binary, u32 size, s8 rev_endian);
t_list *binary_string_to_block_lst(char *str, u32 block_size, u32 last_block_size);

/**
 * @brief Rotate bits to the left
 * @param val value to rotate
 * @param shift number of bits to shift
 * @return rotated value
*/
#define ROTATE_LEFT(val, shift) (((val) << (shift)) | ((val) >> (32 - shift)))

/**
 * @brief Macro to convert a digit to a binary string
 * @param digit digit to convert
 * @param size size of the digit (in bits)
 * @param result store allocated char * digit converted to binary (output)
 */
#define DIGIT_TO_BINSTR(digit, size, result) \
do { \
    char *__binstr__ = malloc((size) + 1); \
	u64 __tmp_digit__ = (digit); \
    if (!__binstr__) { \
        ft_printf_fd(2, "Error: DIGIT_TO_BIN_STR: malloc failed\n"); \
        result = NULL; \
    } else { \
        __binstr__[size] = '\0'; \
        for (s32 i = (size) - 1; i >= 0; i--) { \
			__binstr__[i] = (__tmp_digit__ & 1) + '0'; \
            __tmp_digit__ >>= 1; \
        } \
        result = __binstr__; \
    } \
} while (0)

/* 
	__binstr__[i] = (__tmp_digit__ & 1) + '0'; == __binstr__[i] = (__tmp_digit__ % 2) + '0'
	__tmp_digit__ >>= 1; == __tmp_digit__ /= 2;
*/



#endif /* HEADER_FT_SSL_H */
