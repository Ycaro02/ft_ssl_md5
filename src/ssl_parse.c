#include "../include/ft_ssl.h"
#include "../include/md5.h"
#include "../include/sha256.h"


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

static void ssl_opt_flag_init(char *prg_name, t_flag_context *flag_ctx) {
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

	/* -1 +1 cause we skip first args (algo_mod) */
	flag = parse_flag(argc - 1, argv + 1, flag_ctx, &error);
	if (error == -1) {
		ft_printf_fd(1, "Parse_flag error: %d\n", error);
		return (-1);
	}
	// display_option_list(*flag_ctx);
	return (flag);
}

s32 handle_hash_algo(int argc, char **argv, HashCtx *ctx) {
	if (argc < 2) {
		ft_printf_fd(1, SSL_USAGE_STRING);
		return (1);
	} else if (ftlib_strcmp(argv[1], "md5") == 0) {
		MD5_set_context(ctx);
		argv[1] = "";
	}
	else if (ftlib_strcmp(argv[1], "sha256") == 0) {
		// ft_printf_fd(1, "SHA256 Not implemented\n");
		SHA256_set_context(ctx);
		argv[1] = "";
	} else {
		ft_printf_fd(1, SSL_ERROR_STRING, argv[1], argv[1]);
		return (1);
	}

	if (!(ctx->hash = ft_calloc(sizeof(u32), ctx->hash_size))) {
		return (1);
	}
	return (0);
}
