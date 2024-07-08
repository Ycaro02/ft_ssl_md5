#include "../include/ft_ssl.h"
#include "../include/ssl_test.h"
#include "../libft/parse_flag/parse_flag.h"

#include "../include/md5.h"


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

s32 ssl_handle_flag(int argc, char **argv, t_flag_context *flag_ctx) {
	s32 flag = 0;
	s8	error = 0;

	ssl_opt_flag_init(argv[0], flag_ctx);

	/* Need to adapt to to -1/+1 for skip first args (hash function argument)*/
	// flag = parse_flag(argc, argv, flag_ctx, &error);
	

	
	flag = parse_flag(argc - 1, argv + 1, flag_ctx, &error);
	if (error == -1) {
		ft_printf_fd(1, "Parse_flag error: %d\n", error);
		return (-1);
	}
	display_option_list(*flag_ctx);
	// *input_str = get_opt_value(flag_ctx->opt_lst, flag, S_OPTION);
	return (flag);
}

s32 handle_hash_algo(int argc, char **argv, HashCtx *ctx) {
	if (argc < 2) {
		ft_printf_fd(1, SSL_USAGE_STRING);
		return (1);
	} else if (ftlib_strcmp(argv[1], "md5") == 0) {
		/* set hash algo function need to parse it*/
		MD5_set_context(ctx);
		argv[1] = "";
	}
	else if (ftlib_strcmp(argv[1], "sha256") == 0) {
		ft_printf_fd(1, "SHA256 Not implemented\n");
		argv[1] = "";
		return (1);
	} else {
		ft_printf_fd(1, SSL_ERROR_STRING, argv[1], argv[1]);
		return (1);
	}

	if (!(ctx->hash = ft_calloc(sizeof(u32), ctx->hash_size))) {
		return (1);
	}
	return (0);
}

void free_hash_context(HashCtx *ctx) {
	free_flag_context(&ctx->flag_ctx);
	ft_lstclear(&ctx->input_file, free);
	free(ctx->input_str);
	free(ctx->stdin_str);
	free(ctx->algo_name);
	free(ctx->hash);

}


void hash_process(HashCtx *ctx, char *path, char *str, s8 is_stdin) {
	if (str) {
		ctx->hash_str_func(ctx, (u8 *)str, ft_strlen(str));
	} else {
		if (ctx->hash_file_func(ctx, path) == FALSE) {
			return;
		}
	}

	if (is_stdin) {
		int len = ft_strlen(str);
		if (len > 0 && str[len - 1] == '\n') {
			str[len - 1] = '\0';
		}
		if (has_flag(ctx->flag_val, Q_OPTION)) {
			ft_printf_fd(1, "%s\n", str);
		}
	}

	if (!has_flag(ctx->flag_val, Q_OPTION) && !has_flag(ctx->flag_val, R_OPTION)) {
		if (str && !is_stdin) {
			ft_printf_fd(1, "%s (\"%s\") = ", ctx->algo_name, str);
		} else if (str && is_stdin) {

			ft_printf_fd(1, "(\"%s\") = ", str);
		} else {
			ft_printf_fd(1, "%s (%s) = ", ctx->algo_name, path);
		}
	}
	display_hash(ctx->hash, ctx->hash_size);
	if (!has_flag(ctx->flag_val, Q_OPTION) && has_flag(ctx->flag_val, R_OPTION)) {
		if (str) {
			ft_printf_fd(1, " \"%s\"\n", str);
		} else {
			ft_printf_fd(1, " %s\n", path);
		}
	} else {
		ft_printf_fd(1, "\n");
	}
}


void read_stdin(HashCtx *ctx) {
	u64 size_read = 0;
	char *content = sstring_read_fd(0, NULL, &size_read);
	if (size_read == 0) { /* No content */
		free(content);
		return;
	}
	if (content) {
		ctx->stdin_str = ft_strdup(content);
		ctx->stdin_strlen = ft_strlen(content);
		hash_process(ctx, NULL, ctx->stdin_str, TRUE);
		// ctx->hash_str_func(ctx, (u8 *)ctx->stdin_str, ctx->stdin_strlen);
	}
}


int main(int argc, char **argv) {
	// run_test();

	/* Init hash context used to handle different hash algo */
	HashCtx ctx = {0};

	if (handle_hash_algo(argc, argv, &ctx) != 0) {
		return (1);
	}
	ctx.flag_val = ssl_handle_flag(argc, argv, &ctx.flag_ctx);
	
	/* Read stdin if no argument is given or if -p ctx.flag_val is set */
	if (ctx.flag_val == 0 || has_flag(ctx.flag_val, P_OPTION)) {
		read_stdin(&ctx);
	}
	/* Process -s input */
	if (has_flag(ctx.flag_val, S_OPTION)) {
		ctx.input_str = get_opt_value(ctx.flag_ctx.opt_lst, ctx.flag_val, S_OPTION);
		hash_process(&ctx, NULL, ctx.input_str, FALSE);
		// ctx.input_strlen = ft_strlen(ctx.input_str);
		// ctx.hash_str_func(&ctx, (u8 *)ctx.input_str, ctx.input_strlen);
	}
	
	/* Extract file args in t_list */
	ctx.input_file = extract_args(argc, argv);
	/* process all other args to set input file lst */
	// ft_lstadd_front(&ctx.input_file, ft_lstnew(ft_strdup("ft_ssl")));

	for (t_list *tmp = ctx.input_file; tmp; tmp = tmp->next) {
		hash_process(&ctx, (char *)tmp->content, NULL, FALSE);
		// ctx.hash_file_func(&ctx, (char *)tmp->content);
	}

	free_hash_context(&ctx);

	// ctx.hash_file_func(ctx.input_file->content);
	// MD5_hash_str((u8 *)"abc", ft_strlen("abc"));
	// MD5_hash_str((u8 *)TESTSTRING, ft_strlen(TESTSTRING));
	// MD5_hash_file_read("Makefile");
	// MD5_hash_file_read("src/main.c");
	// MD5_hash_file("ft_ssl");
	return (0);
}