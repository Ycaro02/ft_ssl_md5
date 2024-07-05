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
    // char	cmd[4096];
    char	*cmd = NULL;
	
	u64 total_cmd_len = ((ft_strlen(baseCmd)) + ft_strlen(str) + 1);
	cmd = malloc(total_cmd_len * sizeof(char));
	/* Build cmd */
	ft_bzero(cmd, total_cmd_len);
	sprintf(cmd, baseCmd, str);

	/* Open a pipe to the command */
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
		ft_printf_fd(2, "Error: bash_convert_str: popen failed\n");
        return (NULL);
    }

	/* Read output */
	u32 output_len = ft_strlen(str) * 8 + 1;
    char *output = malloc(output_len + 1);
    if (!output) {
		ft_printf_fd(2, "Error: bash_convert_str: malloc failed\n");
        pclose(fp);
        return (NULL);
    }
    fgets(output, output_len, fp);

    /* clear */
    pclose(fp);

	// ft_printf_fd(1, "cmd: %s\n", cmd);
    // ft_printf_fd(1, "out: %s\n", output);
	free(cmd);

    return (output);
}

void test_string_bash(char *str) {
	char *bash_binary = bash_convert_str(str);
	char *binary = string_to_binary(str, (u64)ft_strlen(str));

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





/**
 * @brief Verify length store in the last block
 * @param block_content block content
 * @param block_len block length
 * @param base_len base length
*/
void verify_length(char *block_content, u64 block_len, u64 base_len) {
	char *length_binary = u64_to_binary(base_len); // Convert length to binary
	assert(ft_strncmp(block_content + block_len - 64, length_binary, 64) == 0);
	// ft_printf_fd(1, ORANGE"Length binary: %s == ", length_binary);
	// ft_printf_fd(1, "Block content len : %s\n"RESET, block_content + block_len - 64);
	free(length_binary);
}

/**
 * @brief Check binary blocks integrity
 * @param input_str input string
 * @param expected_block_count expected block count
*/
void check_binary_blocks(char *input_str, int expected_block_count) {
    t_list *binary_list = binary_string_to_block_lst(input_str);
    int block_count = ft_lstsize(binary_list);
    assert(block_count == expected_block_count);

    t_list *current = binary_list;
    u64 base_len = ft_strlen(input_str);
    u64 input_index = 0;
    int block_str_idx = 0;

	s8 is_padding_1 = TRUE;

    for (int i = 0; i < block_count; i++) {
        char *block_content = (char *)current->content;
        int block_len = ft_strlen(block_content);

        /* Verify block length */
		assert(block_len == MD5_BLOCK_SIZE);

		// ft_printf_fd(1, PINK"Block %d: %s\n"RESET, block_str_idx, block_content);
        /* Verify block content and padding */
        for (int j = 0; j < block_len; j++) {
            if (input_index < base_len) {
                /* Verify actual data from input string */
				assert(input_str[input_index] == block_content[j]);
                input_index++;
            } else {
				/* Verify lenght store in the last block */
				if ((block_str_idx == block_count - 1) && (j == block_len - 64)) {
					verify_length(block_content, block_len, base_len);
					break ;
				}
				/* Verify padding */
				if (is_padding_1) {
					assert(block_content[j] == '1');
					is_padding_1 = FALSE;
				} else {
					assert(block_content[j] == '0');
				}
			}
        }

        block_str_idx++;
        current = current->next;
    }
	free(input_str);
    ft_lstclear(&binary_list, free);
}

void test_binary_string_to_block_lst() {
	// check_binary_blocks(ft_strdup(""), 1);
	check_binary_blocks(ft_strdup("a"), 1);
	check_binary_blocks(ft_strdup(STRING447), 1);
	check_binary_blocks(ft_strdup(STRING448), 2);
	check_binary_blocks(ft_strdup(STRING500), 2);
	check_binary_blocks(ft_strdup(STRING511), 2);
	check_binary_blocks(ft_strdup(STRING512), 2);
	check_binary_blocks(ft_strdup(STRING513), 2);
	check_binary_blocks(ft_strdup(STRING1023), 3);
	check_binary_blocks(ft_strdup(STRING1024), 3);
	check_binary_blocks(string_to_binary(TESTSTRING, ft_strlen(TESTSTRING)), 2);
	test_passed(__func__);
}

void run_test() {
	test_char_to_binary();
	test_atoi_base();
	test_string_to_binary();
	test_u64_to_binary();
	test_binary_string_to_block_lst();
}



// human test
// void test_string_to_block(char *str) {
// 	char *allocated_str = ft_strdup(str);
// 	t_list *lst = binary_string_to_block_lst(allocated_str);
// 	t_list *tmp = lst;
// 	ft_printf_fd(1, RED"Test string len: %u -> "RESET, ft_strlen(str));
// 	ft_printf_fd(1, PINK"List size: %d\n"RESET""YELLOW"Data:\n"RESET, ft_lstsize(lst));
// 	while (tmp) {
// 		char *tmp_str = (char *)tmp->content;
// 		ft_printf_fd(1, "%s -> len %u\n", tmp_str, ft_strlen(tmp_str));
// 		tmp = tmp->next;
// 	}
// 	free(allocated_str);
// 	ft_lstclear(&lst, &free);
// }
