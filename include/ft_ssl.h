#ifndef HEADER_FT_SSL_H
#define HEADER_FT_SSL_H

#include "../libft/libft.h"
#include "../libft/parse_flag/parse_flag.h"


#define SSL_USAGE_STRING "usage: ft_ssl command [flags] [file/string]\n"

#define SSL_ERROR_STRING "./ft_ssl %s\n\
ft_ssl: Error: '%s' is an invalid command.\n\n\
Commands:\n\
md5\n\
sha256\n\n\
Flags:\n\
-p -q -r -s\n"\

typedef t_flag_context FlagCtx;

typedef struct s_hash_context HashCtx;

struct s_hash_context {
	FlagCtx			flag_ctx;								/* Flag context */
	char			*input_str;								/* Input string provided by -s args*/
	u64				 input_strlen;							/* Input string length */
	char			*stdin_str;								/* STDIN string */
	u64				 stdin_strlen;							/* STDIN string length */
	t_list			*input_file;							/* Input file */
	void 			(*hash_file_func)(HashCtx *ctx, char *path);			/* Function to hash file */
	void 			(*hash_str_func)(HashCtx *ctx, u8 *str, u64 len);		/* Function to hash string */
	char			*algo_name;								/* Algorithm name for output message */
	u32				*hash;									/* Hash (digest) */	
	u32				hash_size;								/* Hash size */	
};

/* binary_utils.c */
char	*string_to_binary(u8 *str, u64 len);
u32		binary_string_to_u32(char *binary, u32 size, s8 rev_endian);
t_list *binary_string_to_block_lst(char *str, u32 block_size, u32 last_block_size);

FT_INLINE void display_hash(u32 *hash, u32 hash_size) {
    u32 byte = 0;
	ft_printf_fd(1, ORANGE"Hash: "RESET""YELLOW);
	
	for (u32 i = 0; i < hash_size; i++) {
        for (s32 shift = 24; shift >= 0; shift -= 8) {
            byte = (hash[i] >> shift) & 0xff;
			if (byte < 16) {
				ft_printf_fd(1, "0");
			}
			ft_printf_fd(1, "%x", byte);
		}
    }
	ft_printf_fd(1, RESET"\n");
}

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
