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

void test_u64_to_binary() {
    char *binary = u64_to_binary(1);
    assert(ftlib_strcmp(binary, "0000000000000000000000000000000000000000000000000000000000000001") == 0);
    free(binary);
    
    binary = u64_to_binary(2);
    assert(ftlib_strcmp(binary, "0000000000000000000000000000000000000000000000000000000000000010") == 0);
    free(binary);
    
    binary = u64_to_binary(1023);
    assert(ftlib_strcmp(binary, "0000000000000000000000000000000000000000000000000000001111111111") == 0);
    free(binary);
    
    binary = u64_to_binary(4294967295);
    assert(ftlib_strcmp(binary, "0000000000000000000000000000000011111111111111111111111111111111") == 0);
    free(binary);
    
    test_passed(__func__);
}

#include <stdio.h>

/* rsc/sh/str2bin.sh convert $1 to bin, we need to get the output */
char *bash_convert_str(char *str) {
    char	*baseCmd = "bash rsc/sh/str2bin.sh \"%s\"";
    char	cmd[4096];
	
	/* Build cmd */
	ft_bzero(cmd, 4096);
	sprintf(cmd, baseCmd, str);

	/* Open a pipe to the command */
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
		ft_printf_fd(2, "Error: bash_convert_str: popen failed\n");
        return (NULL);
    }

	/* Read output */
    char *output = malloc(4096);
    if (!output) {
		ft_printf_fd(2, "Error: bash_convert_str: malloc failed\n");
        pclose(fp);
        return (NULL);
    }
    fgets(output, 4096, fp);

    /* clear */
    pclose(fp);

	// ft_printf_fd(1, "cmd: %s\n", cmd);
    // ft_printf_fd(1, "out: %s\n", output);

    return (output);
}

void test_string_bash(char *str) {
	char *bash_binary = bash_convert_str(str);
	char *binary = string_to_binary(str);

	assert(ftlib_strcmp(binary, bash_binary) == 0);
	free(binary);
	free(bash_binary);
}

void test_string_to_binary() {
	test_string_bash("hello");
	test_string_bash("world");
	test_string_bash("1142");
	test_string_bash("42hello");
	test_string_bash("hello42");
	test_string_bash("hello world");
	test_string_bash("bonjour");
	test_string_bash("BonJour");
	test_string_bash("42");
	test_string_bash("a");
	test_passed(__func__);
}

void run_test() {
	test_char_to_binary();
	test_atoi_base();
	test_string_to_binary();
	test_u64_to_binary();
}