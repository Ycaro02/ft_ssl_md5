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
	s8				(*hash_file_func)(HashCtx *ctx, char *path);			/* Function to hash file */
	void 			(*hash_str_func)(HashCtx *ctx, u8 *str, u64 len);		/* Function to hash string */
	char			*algo_name;								/* Algorithm name for output message */
	u32				*hash;									/* Hash (digest) */	
	u32				hash_size;								/* Hash size */
	s32				flag_val;								/* Flag value */
};

/* Block size in bytes (512 bits) */
#define BYTES_BLOCK_SIZE 64

/* Last block size in bytes (448 bits) 512 - 64 for len */
#define BYTES_LAST_BLOCK_SIZE 56

/**
 * @brief Rotate bits to the left
 * @param val value to rotate
 * @param shift number of bits to shift
 * @return rotated value
*/
#define ROTATE_LEFT(val, shift) (((val) << (shift)) | ((val) >> (32 - shift)))

/* Prepare block */
t_list *build_block_list(u8 *padded, u64 len);
void	block_to_u32(u8 *block, u32 *output);

#endif /* HEADER_FT_SSL_H */
