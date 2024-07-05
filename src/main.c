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


s8 handle_flag(int argc, char **argv, t_flag_context *flag_ctx) {

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

	return (TRUE);
}


int main(int argc, char **argv) {
	run_test();

	t_flag_context flag_ctx = {0};

	handle_flag(argc, argv, &flag_ctx);


	MD5_process((u8 *)"abc", ft_strlen("abc"));
	MD5_process((u8 *)TESTSTRING, ft_strlen(TESTSTRING));


	MD5_hash_file("Makefile");
	MD5_hash_file("src/main.c");
	MD5_hash_file("ft_ssl");

	return (0);
}