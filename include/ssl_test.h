#ifndef HEADER_SSL_TEST_H
#define HEADER_SSL_TEST_H

#include <assert.h>
#include "../libft/libft.h"

/* test.c */
void	test_char_to_binary();
void	test_atoi_base();

FT_INLINE void test_passed(const char *func) {
	ft_printf_fd(1, GREEN"All tests passed for "RESET""ORANGE"%s\n"RESET, func);
}

#endif /* HEADER_SSL_TEST_H */
