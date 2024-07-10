#include "../include/ft_ssl.h"
#include "../include/md5.h"
#include "../include/sha256.h"

/**
 * @brief Get option value from flag context
 * @param opt_lst option list
 * @param flag flag value
 * @param to_find flag to find
 * @return allocated copy option value or NULL if not found
 */
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

/**
 * @brief Add different flag accepted by ssl
 * @param prg_name program name
 * @param flag_ctx flag context
 */
static void ssl_opt_flag_init(char *prg_name, t_flag_context *flag_ctx) {
	flag_ctx->prg_name = prg_name;
	add_flag_option(flag_ctx, P_FLAG_CHAR, P_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "print");
	add_flag_option(flag_ctx, Q_FLAG_CHAR, Q_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "quiet");
	add_flag_option(flag_ctx, R_FLAG_CHAR, R_OPTION, OPT_NO_VALUE, OPT_NO_VALUE, "reverse");
	add_flag_option(flag_ctx, S_FLAG_CHAR, S_OPTION, (UINT32_MAX - 1), CHAR_VALUE, "string");
}

/**
 * @brief Parse flag for ssl
 * @param argc number of argument
 * @param argv argument list
 * @param flag_ctx flag context
 * @return flag value or -1 if error
*/
s32 ssl_handle_flag(int argc, char **argv, t_flag_context *flag_ctx) {
	s32 flag = 0;
	s8	error = 0;

	ssl_opt_flag_init(argv[0], flag_ctx);

	/* -1 +1 cause we skip first args (algo_mod) */
	flag = parse_flag(argc - 1, argv + 1, flag_ctx, &error);
	if (error == -1) {
		// ft_printf_fd(1, "Parse_flag error: %d\n", error);
		return (-1);
	}
	// display_option_list(*flag_ctx);
	return (flag);
}

/**
 * @brief Free implemented algorithm node
 * @param content ImplementedAlgo node
*/
void free_implemented_algo(void *content) {
	ImplementedAlgo *algo = (ImplementedAlgo *)content;

	free(algo->name);
	free(algo);
}


/**
 * @brief Create a new implemented algorithm node
 * @param name algorithm name
 * @param func context function
 * @return allocated ImplementedAlgo node
*/
ImplementedAlgo *implementent_algo_create(char *name, ContextFunction func) {
	ImplementedAlgo *algo = NULL;

	if (!(algo = ft_calloc(sizeof(ImplementedAlgo), 1))) {
		return (NULL);
	}
	algo->name = ft_strdup(name);
	if (algo->name == NULL) {
		free(algo);
		return (NULL);
	}
	algo->context_func = func;
	return (algo);
}

/**
 * @brief Add implemented algorithm to the list
 * @param algo_list list of implemented algorithm
 * @param algo ImplementAlgo node (name and set context function)
*/
void implemented_algo_add(t_list **algo_list, ImplementedAlgo *algo) {
	t_list *algo_node = NULL;

	if (!(algo_node = ft_lstnew(algo))) {
		free(algo->name);
		free(algo);
		return ;
	}
	ft_lstadd_back(algo_list, algo_node);
}

/**
 * @brief Build the implemented algorithm list
 * @param algo_list list of implemented algorithm
 * Just need to add one line here to add a new algorithm
*/
void build_implemented_algo_list(t_list **algo_list) {
	implemented_algo_add(algo_list, implementent_algo_create("md5", MD5_set_context));
	implemented_algo_add(algo_list, implementent_algo_create("sha256", SHA256_set_context));
}

/**
 * @brief Handle hash algorithm
 * @param argc number of argument
 * @param argv argument list
 * @param ctx hash context
 * @return TRUE if success, FALSE otherwise
*/
s32 handle_hash_algo(int argc, char **argv, HashCtx *ctx) {
	t_list 			*algo_list = NULL, *tmp = NULL;
	ImplementedAlgo	*algo_node = NULL;
	s32 			ret = TRUE;

	if (argc < 2) {
		ft_printf_fd(1, SSL_USAGE_STRING);
		return (FALSE);
	} 

	build_implemented_algo_list(&algo_list);
	for (tmp = algo_list; tmp; tmp = tmp->next) {
		algo_node = (ImplementedAlgo *)tmp->content;
		if (ftlib_strcmp(argv[1], algo_node->name) == 0) {
			algo_node->context_func(ctx);
			argv[1] = "";
			break ;
		}
	}
	if (ctx->algo_name == NULL) {
		ft_printf_fd(1, SSL_ERROR_STRING, argv[1], argv[1]);
		ret = FALSE;
	} else if (!(ctx->hash = ft_calloc(sizeof(u32), ctx->hash_size))) {
		ret = FALSE;
	}
	ft_lstclear(&algo_list, free_implemented_algo);
	return (ret);
}
