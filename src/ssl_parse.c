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


void set_context_by_id(HashCtx *ctx, u32 id) {
	ContextFunction context_funcs[IMPLEMENTED_ALGO_COUNT] = CONTEXT_FUNC_ARRAY;

	/* Call context funcion */
	context_funcs[id](ctx);

}

char **implemented_algo_get(void) {
	char **implemented = NULL;
	if (!(implemented = ft_calloc(sizeof(char *), IMPLEMENTED_ALGO_COUNT + 1))) {
		return (NULL);
	}
	for (s32 i = 0; i < IMPLEMENTED_ALGO_COUNT; i++) {
		implemented[i] = ft_strdup(IMPLEMENTED_ALGO[i]);
		if (implemented[i] == NULL) {
			free_incomplete_array((void **)implemented, i);
			return (NULL);
		}
	}
	return (implemented);
}

s32 handle_hash_algo(int argc, char **argv, HashCtx *ctx) {
	char **algo_implemented = NULL;

	if (argc < 2) {
		ft_printf_fd(1, SSL_USAGE_STRING);
		return (1);
	} 

	/* Get implemented algo string */
	if (!(algo_implemented = implemented_algo_get())) {
		return (1);
	}

	/* Check if algo is implemented and call context init function */
	for (s32 i = 0; i < IMPLEMENTED_ALGO_COUNT; i++) {
		if (ftlib_strcmp(argv[1], algo_implemented[i]) == 0) {
			set_context_by_id(ctx, i);
			argv[1] = "";
			break;
		}
	}

	free_incomplete_array((void **)algo_implemented, IMPLEMENTED_ALGO_COUNT);

	if (ctx->algo_name == NULL) {
		ft_printf_fd(1, SSL_ERROR_STRING, argv[1], argv[1]);
		return (1);
	} else if (!(ctx->hash = ft_calloc(sizeof(u32), ctx->hash_size))) {
		return (1);
	}
	return (0);
}
