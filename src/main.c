#include "../include/ft_ssl.h"
#include "../include/md5.h"

/**
 * @brief Free hash context
 * @param ctx Hash context to free
*/
void free_hash_context(HashCtx *ctx) {
	free_flag_context(&ctx->flag_ctx);
	ft_lstclear(&ctx->input_file, free);
	free(ctx->input_str);
	free(ctx->stdin_str);
	free(ctx->algo_name);
	free(ctx->hash);

}

/**
 * @brief Hash file content
 * @param ctx Hash context
 * @param path File path
 * @return TRUE if file is hashed, FALSE otherwise
*/
s8 hash_file(HashCtx *ctx, char *path) {
	char	*file_content = NULL, *algo_name = NULL;
	u64		file_size = 0;

	algo_name = str_tolower(ctx->algo_name);
	if (!(file_content = sstring_read_fd(-1, path, &file_size))) {
		ft_printf_fd(1, "ft_ssl: %s: %s: No such file or directory\n", algo_name, path);
		free(algo_name);
		return (FALSE);
	}
	ctx->hash_func(ctx, (u8 *)file_content, file_size);
	free(file_content);
	free(algo_name);
	return (TRUE);
}

/**
 * @brief Process hash
 * @param ctx Hash context
 * @param path File path
 * @param str String to hash
 * @param is_stdin Is stdin
 */
void hash_process(HashCtx *ctx, char *path, char *str, s8 is_stdin) {
	if (str) {
		ctx->hash_func(ctx, (u8 *)str, ft_strlen(str));
	} else if (!hash_file(ctx, path)) {
		return;
	}

	output_display(ctx, path, str, is_stdin);
}

/**
 * @brief Read stdin and hash it
 * @param ctx Hash context
 */
void read_stdin(HashCtx *ctx) {
	u64 size_read = 0;
	char *content = sstring_read_fd(0, NULL, &size_read);
	if (content) {
		ctx->stdin_str = ft_strdup(content);
		ctx->stdin_strlen = ft_strlen(content);
		hash_process(ctx, NULL, ctx->stdin_str, TRUE);
		free(content);
	}
}

/**
 * @brief ft_ssl main function
 * @param argc Argument count
 * @param argv Argument list
 */
s32 ft_ssl(int argc, char **argv) {
	HashCtx ctx = {0};

	if (!handle_hash_algo(argc, argv, &ctx)) {
		return (1);
	} else if ((ctx.flag_val = ssl_handle_flag(argc, argv, &ctx.flag_ctx)) == -1) {
		free_hash_context(&ctx);
		return (1);
	}

	if (has_flag(ctx.flag_val, S_OPTION)) {
		ctx.input_str = get_opt_value(ctx.flag_ctx.opt_lst, ctx.flag_val, S_OPTION);
	}
	
	/* Extract file args in t_list */
	ctx.input_file = extract_args(argc, argv);
	
	/* Read stdin if no argument is given or if -p is set */
	if ((ft_lstsize(ctx.input_file) == 0 && ctx.input_str == NULL) || has_flag(ctx.flag_val, P_OPTION)) {
		read_stdin(&ctx);
	}

	/* Process input string (-s) */
	if (ctx.input_str) {
		hash_process(&ctx, NULL, ctx.input_str, FALSE);
	}

	/* process all file args */
	for (t_list *tmp = ctx.input_file; tmp; tmp = tmp->next) {
		hash_process(&ctx, (char *)tmp->content, NULL, FALSE);
	}

	free_hash_context(&ctx);
	return (0);
}

int main(int argc, char **argv) {
	int ret = 0;

	ret = ft_ssl(argc, argv);
	return (ret);
}