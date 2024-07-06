#include "../include/ft_ssl.h"
#include "../include/ssl_test.h"

#include "../libft/parse_flag/parse_flag.h"

/*
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

void *get_opt_value(t_list *opt_lst, uint32_t flag, uint32_t to_find)
{
	t_opt_node	*opt = NULL;
	void		*ret = NULL;

	if (has_flag(flag, to_find)) {
		opt = search_exist_opt(opt_lst, is_same_flag_val_opt, (void *)&to_find);
		if (opt && opt->value_type == DECIMAL_VALUE) {
			ret = malloc(sizeof(u32));
			*(u32 *)ret = opt->val.digit;
		} else if (opt && (opt->value_type == HEXA_VALUE || opt->value_type == CHAR_VALUE)) {
			ret = ft_strdup(opt->val.str);
		}
	}
	return (ret);
}

void ssl_opt_flag_init(char *prg_name, t_flag_context *flag_ctx) {
	flag_ctx->prg_name = prg_name;
	add_flag_option(flag_ctx, P_FLAG_CHAR, P_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "print");
	add_flag_option(flag_ctx, Q_FLAG_CHAR, Q_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "quiet");
	add_flag_option(flag_ctx, R_FLAG_CHAR, R_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "reverse");
	add_flag_option(flag_ctx, S_FLAG_CHAR, S_OPTION, (UINT32_MAX - 1), CHAR_VALUE, "string");
}

s8 ssl_handle_flag(int argc, char **argv, t_flag_context *flag_ctx, char **input_str) {
	s8 error = 0;
	u32 flag = 0;


	ssl_opt_flag_init(argv[0], flag_ctx);

	/* Need to adapt to to -1/+1 for skip first args (hash function argument)*/
	flag = parse_flag(argc, argv, flag_ctx, &error);
	if (error == -1) {
		ft_printf_fd(1, "Parse_flag error: %d\n", error);
		return (FALSE);
	}
	(void)flag;

	display_option_list(*flag_ctx);

	*input_str = get_opt_value(flag_ctx->opt_lst, flag, S_OPTION);

	return (TRUE);
}

int cmp_large_str(u8 *s1, u8 *s2, u64 size) {
	for (u64 i = 0; i < size; i++) {
		if (s1[i] != s2[i]) {
			return (1);
		}
	}
	return (0);
}

/**
 * @brief Hash a file with MD5 algorithm
 * @param path file path
*/
void MD5_hash_file(char *path) {
	u64		file_size = 0;
	char	*file_map = sstring_read_fd(-1, path, &file_size);
	
	

	// To remove
	u8		*map = mmap_file(path, &file_size, 0);
	u64 	map_file_size = 0;
	if (cmp_large_str((u8 *)file_map, map, map_file_size) == 0) {
		ft_printf_fd(1, GREEN"Same %s size: %u\n"RESET, path, file_size);
	} else {
		ft_printf_fd(1, RED"Not Same %s size: %u\n"RESET, path, file_size);
	}
	if (map) {
		ft_printf_fd(1, YELLOW"mmap load File %s size: %u\n"RESET, path, file_size);
		MD5_process(map, file_size);
		munmap(map, map_file_size);
	}
	// end to remove

	if (file_map) {
		ft_printf_fd(1, PINK"sstring_read_fd load File %s size: %u\n"RESET, path, file_size);
		MD5_process((u8 *)file_map, file_size);
		free(file_map);
	}
}

void set_stdin_noblock() {
	s32 stdin_flag = fcntl(0, F_GETFL, 0);
	if (stdin_flag == -1) {
		ft_printf_fd(2, "Error: fcntl getFlag for stdin\n");
		return;
	}
	if (fcntl(0, F_SETFL, stdin_flag | O_NONBLOCK) == -1) {
		ft_printf_fd(2, "Error: fcntl setFlag for stdin\n");
		return;
	}
}

void read_stdin() {
	u64 size_read = 0;
	char *content = sstring_read_fd(0, NULL, &size_read);
	if (size_read == 0) { /* No content */
		free(content);
		return;
	}
	if (content) {
		ft_printf_fd(1, "STDIN content: %s -> size %u\n", content, size_read);
		MD5_process((u8 *)content, size_read);
	}
}

int main(int argc, char **argv) {
	// run_test();

	t_flag_context flag_ctx = {0};

	set_stdin_noblock();
	read_stdin();

	char *s_flag_input = NULL;
	ssl_handle_flag(argc, argv, &flag_ctx, &s_flag_input);
	if (s_flag_input) {
		MD5_process((u8 *)s_flag_input, ft_strlen(s_flag_input));
	}

	// MD5_process((u8 *)"abc", ft_strlen("abc"));
	// MD5_process((u8 *)TESTSTRING, ft_strlen(TESTSTRING));
	// MD5_hash_file_read("Makefile");
	// MD5_hash_file_read("src/main.c");
	MD5_hash_file("ft_ssl");


	return (0);
}