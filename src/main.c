#include "../include/ft_ssl.h"
#include "../include/ssl_test.h"

#include "../libft/parse_flag/parse_flag.h"

#define FLAG_P	1

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

void set_opt_value(t_list *opt_lst, uint32_t flag, uint32_t to_find, void *to_update)
{
	if (has_flag(flag, to_find)) {
		t_opt_node *opt = search_exist_opt(opt_lst, is_same_flag_val_opt, (void *)&to_find);
		// ft_printf_fd(1, "for tofind %u: opt: %p\n", to_find, opt);
		if (!opt) {
			return ;
		}
		if (opt->value_type == DECIMAL_VALUE) {
			*(uint32_t *)to_update = opt->val.digit;
		} else if (opt->value_type == HEXA_VALUE || opt->value_type == CHAR_VALUE) {
			*(char **)to_update = ft_strdup(opt->val.str);
			// ft_printf_fd(1, "to_update: %s\n", *(char **)to_update);
		}
	}
}

s8 handle_flag(int argc, char **argv, t_flag_context *flag_ctx, char **input_str) {

	add_flag_option(flag_ctx, P_FLAG_CHAR, P_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "print");
	add_flag_option(flag_ctx, Q_FLAG_CHAR, Q_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "quiet");
	add_flag_option(flag_ctx, R_FLAG_CHAR, R_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "reverse");
	add_flag_option(flag_ctx, S_FLAG_CHAR, S_OPTION, (UINT32_MAX - 1), CHAR_VALUE, "string");

	s8 error = 0;
	u32 flag = parse_flag(argc, argv, flag_ctx, &error);
	if (error == -1) {
		ft_printf_fd(1, "Parse_flag error: %d\n", error);
		return (FALSE);
	}
	(void)flag;

	display_option_list(*flag_ctx);

	set_opt_value(flag_ctx->opt_lst, flag, S_OPTION, input_str);

	return (TRUE);
}


int main(int argc, char **argv) {
	run_test();

	t_flag_context flag_ctx = {0};

	char *s_flag_input = NULL;
	handle_flag(argc, argv, &flag_ctx, &s_flag_input);
	if (s_flag_input) {
		MD5_process((u8 *)s_flag_input, ft_strlen(s_flag_input));
	}


	// MD5_process((u8 *)"abc", ft_strlen("abc"));
	// MD5_process((u8 *)TESTSTRING, ft_strlen(TESTSTRING));
	// MD5_hash_file("Makefile");
	// MD5_hash_file("src/main.c");
	// MD5_hash_file("ft_ssl");

	return (0);
}