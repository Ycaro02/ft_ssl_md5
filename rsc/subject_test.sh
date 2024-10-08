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


# MD5 subjects tests
display_color_msg ${YELLOW} "MD5 Subjects tests"

# Test 1
CMD=$(echo "42 is nice" | ${SSL_BIN} md5)
check_output "(stdin)= 35f1d6de0302e2086a4e472266efb3a9" "${CMD}"

# Test 2
CMD=$(echo "42 is nice" | ${SSL_BIN} md5 -p)
check_output "(\"42 is nice\")= 35f1d6de0302e2086a4e472266efb3a9" "${CMD}"

# Test 3
CMD=$(echo "Pity the living." | ${SSL_BIN} md5 -q -r)
check_output "e20c3b973f63482a778f3fd1869b7f25" "${CMD}"

# Test 4
echo "And above all," > file
CMD=$(${SSL_BIN} md5 file)
check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${CMD}"

# Test 5
CMD=$(${SSL_BIN} md5 -r file)
check_output "53d53ea94217b259c11a5a2d104ec58a file" "${CMD}"

# Test 6
CMD=$(${SSL_BIN} md5 -s "pity those that aren't following baerista on spotify.")
check_output "MD5 (\"pity those that aren't following baerista on spotify.\") = a3c990a1964705d9bf0e602f44572f5f" "${CMD}"

# Test 7
CMD=$(echo "be sure to handle edge cases carefully" | ${SSL_BIN} md5 -p file)
check_output "(\"be sure to handle edge cases carefully\")= 3553dc7dc5963b583c056d1b9fa3349c\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${CMD}"

# Test 8
CMD=$(echo "some of this will not make sense at first" | ${SSL_BIN} md5 file)
check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${CMD}"

# Test 9
CMD=$(echo "but eventually you will understand" | ${SSL_BIN} md5 -p -r file)
check_output "(\"but eventually you will understand\")= dcdd84e0f635694d2a943fa8d3905281\n53d53ea94217b259c11a5a2d104ec58a file" "${CMD}"

# Test 10
CMD=$(echo "GL HF let's go" | ${SSL_BIN} md5 -p -s "foo" file)
check_output "(\"GL HF let's go\")= d1e3cc342b6da09480b27ec57ff243e2\nMD5 (\"foo\") = acbd18db4cc2f85cedef654fccc4a4d8\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "${CMD}"

# Test 11
CMD=$(echo "one more thing" | ${SSL_BIN} md5 -r -p -s "foo" file -s "bar")
check_output "(\"one more thing\")= a0bd1876c6f011dd50fae52827f445f5\nacbd18db4cc2f85cedef654fccc4a4d8 \"foo\"\n53d53ea94217b259c11a5a2d104ec58a file\nft_ssl: md5: -s: No such file or directory\nft_ssl: md5: bar: No such file or directory" "${CMD}"

# Test 12
CMD=$(echo "just to be extra clear" | ${SSL_BIN} md5 -r -q -p -s "foo" file)
check_output "just to be extra clear\n3ba35f1ea0d170cb3b9a752e3360286c\nacbd18db4cc2f85cedef654fccc4a4d8\n53d53ea94217b259c11a5a2d104ec58a" "${CMD}"

display_color_msg ${MAGENTA} "All MD5 subjects tests passed."
rm -f expected output file


# SHA256 subjects tests
display_color_msg ${YELLOW} "SHA256 Subjects tests"

TEST_ID=0

# test 1 
echo "https://www.42.fr/" > website
CMD=$(${SSL_BIN} sha256 -q website)
check_output "1ceb55d2845d9dd98557b50488db12bbf51aaca5aa9c1199eb795607a2457daf" "${CMD}"

# test 2
CMD=$(${SSL_BIN} sha256 -s "42 is nice")
check_output "SHA256 (\"42 is nice\") = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f" "${CMD}"
display_color_msg ${MAGENTA} "All SHA256 subjects tests passed."



# Test with ft_ssl binary (advanced tests)
display_color_msg ${YELLOW} "Test with ft_ssl binary"
TEST_ID=0

# test 1 
display_color_msg ${YELLOW} "MD5:"
CMD=$(${SSL_BIN} md5 ft_ssl -q)
OUTPUT=$(md5sum ft_ssl | awk '{print $1}')
check_output "${OUTPUT}" "${CMD}"


# test 2
display_color_msg ${YELLOW} "SHA256:"
CMD=$(${SSL_BIN} sha256 ft_ssl -q)
OUTPUT=$(sha256sum ft_ssl | awk '{print $1}')
check_output "${OUTPUT}" "${CMD}"

display_color_msg ${MAGENTA} "All tests passed."

rm -f expected output website