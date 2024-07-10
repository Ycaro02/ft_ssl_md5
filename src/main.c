#include "../include/ft_ssl.h"
#include "../include/md5.h"


void free_hash_context(HashCtx *ctx) {
	free_flag_context(&ctx->flag_ctx);
	ft_lstclear(&ctx->input_file, free);
	free(ctx->input_str);
	free(ctx->stdin_str);
	free(ctx->algo_name);
	free(ctx->hash);

}

void display_prefix(HashCtx *ctx, char *str , char *path, s8 is_stdin) {
	
	if (is_stdin && !has_flag(ctx->flag_val, Q_OPTION)) {
		if (!has_flag(ctx->flag_val, P_OPTION)) {
			ft_printf_fd(1, "(stdin)= ");
		} else {
			ft_printf_fd(1, "(\"%s\")= ", str);
		}
		return ;
	}
	/* Classic display */
	if (!has_flag(ctx->flag_val, Q_OPTION) && !has_flag(ctx->flag_val, R_OPTION)) {
		if (str && !is_stdin) {
			ft_printf_fd(1, "%s (\"%s\") = ", ctx->algo_name, str);
		} else if (!str) { /* File case */
			ft_printf_fd(1, "%s (%s) = ", ctx->algo_name, path);
		}
	}
}

void display_suffix(HashCtx *ctx, char *str , char *path, s8 is_stdin) {
	/* Reverse display (display info data after hash)*/
	if (!has_flag(ctx->flag_val, Q_OPTION) && has_flag(ctx->flag_val, R_OPTION) && !is_stdin) {
		if (str) {
			ft_printf_fd(1, " \"%s\"\n", str);
		} else {
			ft_printf_fd(1, " %s\n", path);
		}
	} else {
		ft_printf_fd(1, "\n");
	}
}

void display_hash(u32 *hash, u32 hash_size) {
    u32 byte = 0;
	for (u32 i = 0; i < hash_size; i++) {
        for (s32 shift = 24; shift >= 0; shift -= 8) {
            byte = (hash[i] >> shift) & 0xff;
			if (byte < 16) {
				ft_printf_fd(1, "0");
			}
			ft_printf_fd(1, "%x", byte);
		}
    }
}

char *str_no_caps(char *str) {
	char *no_caps = ft_strdup(str);
	u64 len = ft_strlen(str);
	for (u64 i = 0; i < len; i++) {
		if (no_caps[i] >= 'A' && no_caps[i] <= 'Z') {
			no_caps[i] = ft_tolower(str[i]);
		}
	}
	return (no_caps);
}

s8 hash_file(HashCtx *ctx, char *path) {
	char	*file_content = NULL, *algo_name = NULL;
	u64		file_size = 0;

	algo_name = str_no_caps(ctx->algo_name);
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

void handle_stdin_input(HashCtx *ctx, char *str) {
	/* Cut last \n if is stdin */
	u64 len = ctx->stdin_strlen;
	if (len > 0 && str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
	/* Quiet mode with stdin (-P) */
	if (has_flag(ctx->flag_val, Q_OPTION) && has_flag(ctx->flag_val, P_OPTION)) {
		ft_printf_fd(1, "%s\n", str);
	}
}

void hash_process(HashCtx *ctx, char *path, char *str, s8 is_stdin) {
	if (str) {
		ctx->hash_func(ctx, (u8 *)str, ft_strlen(str));
	} else {
		if (!hash_file(ctx, path)) {
			return;
		}
	}

	if (is_stdin) {
		handle_stdin_input(ctx, str);
	}
	/* Display output */
	display_prefix(ctx, str, path, is_stdin);
	display_hash(ctx->hash, ctx->hash_size);
	display_suffix(ctx, str, path, is_stdin);

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
		free(content);
	}
}

int main(int argc, char **argv) {
	HashCtx ctx = {0};

	if (handle_hash_algo(argc, argv, &ctx) != TRUE) {
		return (1);
	}
	ctx.flag_val = ssl_handle_flag(argc, argv, &ctx.flag_ctx);
	if (has_flag(ctx.flag_val, S_OPTION)) {
		ctx.input_str = get_opt_value(ctx.flag_ctx.opt_lst, ctx.flag_val, S_OPTION);
	}
	
	/* Extract file args in t_list */
	ctx.input_file = extract_args(argc, argv);
	
	/* Read stdin if no argument is given or if -p is set */
	if ((ft_lstsize(ctx.input_file) == 0 && ctx.input_str == NULL) || has_flag(ctx.flag_val, P_OPTION)) {
		read_stdin(&ctx);
	}
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