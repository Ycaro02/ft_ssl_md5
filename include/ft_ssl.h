#ifndef HEADER_FT_SSL_H
#define HEADER_FT_SSL_H

#include "../libft/libft.h"
#include "../libft/parse_flag/parse_flag.h"


#define SSL_USAGE_STRING "usage: ft_ssl command [flags] [file/string]\n"

#define SSL_HELP_STRING "Valid commands:\n\n\
Commands:\n\
md5\n\
sha256\n\
whirlpool\n\n\
Flags:\n\
-p -q -r -s\n"\

/*
	Read stdin if no argument is given or if -p flag is set
	-p, echo STDIN to STDOUT and append the checksum to STDOUT
	-q, quiet mode
	-r, reverse the format of the output.
	-s, print the sum of the given string
*/

enum e_flag {
	P_OPTION = (1<<0),
	Q_OPTION = (1<<1),
	R_OPTION = (1<<2),
	S_OPTION = (1<<3),
};

#define P_FLAG_CHAR	'p'
#define Q_FLAG_CHAR	'q'
#define R_FLAG_CHAR	'r'
#define S_FLAG_CHAR	's'


typedef t_flag_context			FlagCtx;
typedef struct s_hash_context	HashCtx;

struct s_hash_context {
	FlagCtx			flag_ctx;							/* Flag context */
	char			*input_str;							/* Input string provided by -s args*/
	u64				 input_strlen;						/* Input string length */
	char			*stdin_str;							/* STDIN string */
	u64				 stdin_strlen;						/* STDIN string length */
	t_list			*input_file;						/* Input file */
	void 			(*hash_func)(HashCtx*, u8*, u64);	/* Hash Function */
	char			*algo_name;							/* Algorithm name for output message */
	u32				*hash;								/* Hash (digest) */	
	u32				hash_size;							/* Hash size */
	s32				flag_val;							/* Flag value */
};


/* Context function pointer */
typedef void (*ContextFunction)(HashCtx*);

/* Implemented algorithm structure */
typedef struct s_implemented_algo {
	char			*name;			/* Algorithm name */
	ContextFunction context_func;	/* Context function */
} ImplementedAlgo;

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
#define ROTATE_LEFT_32(val, shift) (((val) << (shift)) | ((val) >> (32 - shift)))

/* ssl_parse */
s32		ssl_handle_flag(int argc, char **argv, t_flag_context *flag_ctx);
s32		handle_hash_algo(int argc, char **argv, HashCtx *ctx);

/* Prepare block */
t_list	*build_block_list(u8 *padded, u64 len, s8 reverse_len_endian, u64 last_block_size);
void	block_to_u32(u8 *block, u32 *output);
void	*get_opt_value(t_list *opt_lst, uint32_t flag, uint32_t to_find);

/* hash display */
void output_display(HashCtx *ctx, char *path, char *str, s8 is_stdin);

#endif /* HEADER_FT_SSL_H */
