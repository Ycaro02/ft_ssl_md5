#include "../include/ssl_test.h"
#include "../include/ft_ssl.h"

void test_atoi_base() {
	assert(ft_atoi_base("101", "01") == 5);
	assert(ft_atoi_base("101", "0123456789") == 101);
	assert(ft_atoi_base("101", "0123456789ABCDEF") == 257);
	assert(ft_atoi_base("101", "0123456789abcdef") == 257);
	assert(ft_atoi_base("-101", "01") == -5);
	assert(ft_atoi_base("5", "0123456789abcdef") == 5);
	assert(ft_atoi_base("59", "0123456789abcdef") == 89);
	assert(ft_atoi_base("FF", "0123456789ABCDEF") == 255);
    assert(ft_atoi_base("-FF", "0123456789ABCDEF") == -255);
    assert(ft_atoi_base("z", "0123456789abcdefghijklmnopqrstuvwxyz") == 35);
    assert(ft_atoi_base("-z", "0123456789abcdefghijklmnopqrstuvwxyz") == -35);
    assert(ft_atoi_base("10", "02") == 0);
    assert(ft_atoi_base("hello", "0123456789abcdef") == 0);
    assert(ft_atoi_base("", "01") == 0);
    assert(ft_atoi_base("101010", "01") == 42);
    assert(ft_atoi_base("7FFFFFFF", "0123456789ABCDEF") == 2147483647);
    assert(ft_atoi_base("-80000000", "0123456789ABCDEF") == -2147483648);
	test_passed(__func__);
}

void test_char_to_binary() {
	char *binary = char_to_binary('a');

	assert(ftlib_strcmp(binary, "01100001") == 0);
	free(binary);
	binary = char_to_binary('b');
	assert(ftlib_strcmp(binary, "01100010") == 0);
	free(binary);
	binary = char_to_binary('y');
	assert(ftlib_strcmp(binary, "01111001") == 0);
	free(binary);
	test_passed(__func__);
}

void run_test() {
	test_char_to_binary();
	test_atoi_base();
}