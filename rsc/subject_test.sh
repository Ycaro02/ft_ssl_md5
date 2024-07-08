#!/bin/bash

TEST_ID=0

SSL_BIN=./ft_ssl

source rsc/sh/color.sh

check_output() {
    expected="${1}"
    output="${2}"
	number=$((${TEST_ID} + 1))
	TEST_ID=${number}

	echo -e "|${expected}|" > expected
	echo -e "|${output}|" > output
	diff expected output

	if [ $? -eq 0 ]; then
		display_color_msg ${GREEN} "Test ${number} passed."
	else
		display_color_msg ${RED} "Test ${number} failed."
	fi
}

display_color_msg ${YELLOW} "MD5 Subjects tests"

# Test 1
output=$(echo "42 is nice" | ${SSL_BIN} md5)
check_output "(stdin)= 35f1d6de0302e2086a4e472266efb3a9" "${output}"

# Test 2
output=$(echo "42 is nice" | ${SSL_BIN} md5 -p)
check_output "(\"42 is nice\")= 35f1d6de0302e2086a4e472266efb3a9" "${output}"

# Test 3
output=$(echo "Pity the living." | ${SSL_BIN} md5 -q -r)
check_output "e20c3b973f63482a778f3fd1869b7f25" "${output}"

# Test 4
echo "And above all," > file
output=$(${SSL_BIN} md5 file)
check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${output}"

# Test 5
output=$(${SSL_BIN} md5 -r file)
check_output "53d53ea94217b259c11a5a2d104ec58a file" "${output}"

# Test 6
output=$(${SSL_BIN} md5 -s "pity those that aren't following baerista on spotify.")
check_output "MD5 (\"pity those that aren't following baerista on spotify.\") = a3c990a1964705d9bf0e602f44572f5f" "${output}"

# Test 7
output=$(echo "be sure to handle edge cases carefully" | ${SSL_BIN} md5 -p file)
check_output "(\"be sure to handle edge cases carefully\")= 3553dc7dc5963b583c056d1b9fa3349c\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${output}"

# Test 8
output=$(echo "some of this will not make sense at first" | ${SSL_BIN} md5 file)
check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${output}"

# Test 9
output=$(echo "but eventually you will understand" | ${SSL_BIN} md5 -p -r file)
check_output "(\"but eventually you will understand\")= dcdd84e0f635694d2a943fa8d3905281\n53d53ea94217b259c11a5a2d104ec58a file" "${output}"

# Test 10
output=$(echo "GL HF let's go" | ${SSL_BIN} md5 -p -s "foo" file)
check_output "(\"GL HF let's go\")= d1e3cc342b6da09480b27ec57ff243e2\nMD5 (\"foo\") = acbd18db4cc2f85cedef654fccc4a4d8\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${output}"

# Test 11
output=$(echo "one more thing" | ${SSL_BIN} md5 -r -p -s "foo" file -s "bar")
check_output "(\"one more thing\")= a0bd1876c6f011dd50fae52827f445f5\nacbd18db4cc2f85cedef654fccc4a4d8 \"foo\"\n53d53ea94217b259c11a5a2d104ec58a file\nft_ssl: md5: -s: No such file or directory\nft_ssl: md5: bar: No such file or directory" "${output}"

# Test 12
output=$(echo "just to be extra clear" | ${SSL_BIN} md5 -r -q -p -s "foo" file)
check_output "just to be extra clear\n3ba35f1ea0d170cb3b9a752e3360286c\nacbd18db4cc2f85cedef654fccc4a4d8\n53d53ea94217b259c11a5a2d104ec58a" "${output}"

display_color_msg ${MAGENTA} "All MD5 subjects tests passed."

rm -f expected output file