#include "../include/ft_ssl.h"
#include "../include/ssl_test.h"




int main() {
	run_test();
	// MD5_process(TESTSTRING);
	MD5_process((u8 *)"abc", ft_strlen("abc"));
	MD5_process((u8 *)TESTSTRING, ft_strlen(TESTSTRING));


	MD5_hash_file("Makefile");
	MD5_hash_file("src/main.c");
	MD5_hash_file("ft_ssl");

	return (0);
}