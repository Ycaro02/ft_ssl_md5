#include "../include/ft_ssl.h"

void display_prefix(HashCtx *ctx, char *str , char *path, s8 is_stdin) {
	
	if (is_stdin && !has_flag(ctx->flag_val, Q_OPTION)) {
		!has_flag(ctx->flag_val, P_OPTION) ? ft_printf_fd(1, "(stdin)= ") : ft_printf_fd(1, "(\"%s\")= ", str);
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

void output_display(HashCtx *ctx, char *path, char *str, s8 is_stdin) {
	if (is_stdin) {
		handle_stdin_input(ctx, str);
	}
	/* Display output */
	display_prefix(ctx, str, path, is_stdin);
	display_hash(ctx->hash, ctx->hash_size);
	display_suffix(ctx, str, path, is_stdin);
}