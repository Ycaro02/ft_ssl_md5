#include "../include/ft_ssl.h"
#include "../include/ssl_test.h"


int main() {
	run_test();
	// MD5_process(TESTSTRING);
	MD5_process("abc");
	MD5_process(TESTSTRING);
	return (0);
}