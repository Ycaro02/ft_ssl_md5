#!/bin/bash

TEST_ID=0

SSL_BIN=./ft_ssl

source rsc/sh/color.sh

function check_output {
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

function mandatory_test {

	# MD5 subjects tests
	display_color_msg ${YELLOW} "MD5 Subjects tests"

	# Test 1
	check_output "(stdin)= 35f1d6de0302e2086a4e472266efb3a9" "$(echo "42 is nice" | ${SSL_BIN} md5)"
	# Test 2
	check_output "(\"42 is nice\")= 35f1d6de0302e2086a4e472266efb3a9" "$(echo "42 is nice" | ${SSL_BIN} md5 -p)"
	# Test 3
	check_output "e20c3b973f63482a778f3fd1869b7f25" "$(echo "Pity the living." | ${SSL_BIN} md5 -q -r)"
	# Test 4
	echo "And above all," > file
	check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "$(${SSL_BIN} md5 file)"
	# Test 5
	check_output "53d53ea94217b259c11a5a2d104ec58a file" "$(${SSL_BIN} md5 -r file)"
	# Test 6
	check_output "MD5 (\"pity those that aren't following baerista on spotify.\") = a3c990a1964705d9bf0e602f44572f5f" "$(${SSL_BIN} md5 -s "pity those that aren't following baerista on spotify.")"
	# Test 7
	check_output "(\"be sure to handle edge cases carefully\")= 3553dc7dc5963b583c056d1b9fa3349c\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "$(echo "be sure to handle edge cases carefully" | ${SSL_BIN} md5 -p file)"
	# Test 8
	check_output "MD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "$(echo "some of this will not make sense at first" | ${SSL_BIN} md5 file)"
	# Test 9
	check_output "(\"but eventually you will understand\")= dcdd84e0f635694d2a943fa8d3905281\n53d53ea94217b259c11a5a2d104ec58a file" "$(echo "but eventually you will understand" | ${SSL_BIN} md5 -p -r file)"
	# Test 10
	check_output "(\"GL HF let's go\")= d1e3cc342b6da09480b27ec57ff243e2\nMD5 (\"foo\") = acbd18db4cc2f85cedef654fccc4a4d8\nMD5 (file) = 53d53ea94217b259c11a5a2d104ec58a" "$(echo "GL HF let's go" | ${SSL_BIN} md5 -p -s "foo" file)"
	# Test 11
	local output=$(echo "one more thing" | ${SSL_BIN} md5 -r -p -s "foo" file -s "bar")
	check_output "(\"one more thing\")= a0bd1876c6f011dd50fae52827f445f5\nacbd18db4cc2f85cedef654fccc4a4d8 \"foo\"\n53d53ea94217b259c11a5a2d104ec58a file\nft_ssl: md5: -s: No such file or directory\nft_ssl: md5: bar: No such file or directory" "${output}"
	# Test 12
	check_output "just to be extra clear\n3ba35f1ea0d170cb3b9a752e3360286c\nacbd18db4cc2f85cedef654fccc4a4d8\n53d53ea94217b259c11a5a2d104ec58a" "$(echo "just to be extra clear" | ${SSL_BIN} md5 -r -q -p -s "foo" file)"

	display_color_msg ${MAGENTA} "All MD5 subjects tests passed."
	rm -f expected output file

	# SHA256 subjects tests
	display_color_msg ${YELLOW} "SHA256 Subjects tests"
	# test 13
	echo "https://www.42.fr/" > website
	check_output "1ceb55d2845d9dd98557b50488db12bbf51aaca5aa9c1199eb795607a2457daf" "$(${SSL_BIN} sha256 -q website)"
	# test 14
	check_output "SHA256 (\"42 is nice\") = b7e44c7a40c5f80139f0a50f3650fb2bd8d00b0d24667c4c2ca32c88e13b758f" "$(${SSL_BIN} sha256 -s "42 is nice")"
	display_color_msg ${MAGENTA} "All SHA256 subjects tests passed."

	# Test with ft_ssl binary (advanced tests)
	display_color_msg ${YELLOW} "Test with ft_ssl binary"

	# test 15 
	echo -ne "${YELLOW}MD5: "
	check_output "$(md5sum ft_ssl | awk '{print $1}')" "$(${SSL_BIN} md5 ft_ssl -q)"
	# test 16
	echo -ne "${YELLOW}SHA256: "
	check_output "$(sha256sum ft_ssl | awk '{print $1}')" "$(${SSL_BIN} sha256 ft_ssl -q)"

	display_color_msg ${MAGENTA} "All mandatory tests passed."
	rm -f expected output website
}


mandatory_test

function whirlpool_hash_file {
	local file="${1}"
	docker run --rm -v${PWD}/:/app whirlpool-test whirlpooldeep /app/"${file}"  | awk '{print $1}'
}

function whirlpool_hash_str {
	local str="${1}"
	docker run --rm -v${PWD}/:/app whirlpool-test bash -c "echo -n ${str} | whirlpooldeep"  | awk '{print $1}'  
}


function test_ft_ssl_whirlpool_file {
	local str="${1}"
	local output=$(./ft_ssl whirlpool "${str}" | awk -F '=' '{print $2}' | tr -d ' ')
	echo -n ${output} 

}

function test_ft_ssl_whirlpool_str {
	local str="${1}"
	local output=$(./ft_ssl whirlpool -s "${str}" | awk -F '=' '{print $2}' | tr -d ' ')
	echo -n ${output} 
}

function wp_test_file {
	local file_name="${1}"
	local expected=$(whirlpool_hash_file ${file_name})
	local output=$(test_ft_ssl_whirlpool_file ${file_name})
	check_output "${expected}" "${output}"
}


function wp_test_str {
	local str="${1}"
	local expected=$(whirlpool_hash_str "${str}")
	local output=$(test_ft_ssl_whirlpool_str "${str}")
	check_output "${expected}" "${output}"
}

function whirlpool_test {
	display_color_msg "${LIGHT_CYAN}" "Build docker image whirlpool-test from ./rsc/whirlpool_test/Dockerfile"
	docker build -t whirlpool-test ./rsc/whirlpool_test > /dev/null 2>&1
	TEST_ID=0
	display_color_msg ${YELLOW} "Whirlpool test"
	# Test 1
	wp_test_file "ft_ssl"
	# Test 2
	wp_test_file "Makefile"
	# Test 3
	wp_test_str "a"
	# Test 4
	wp_test_str "The quick brown fox jumps over the lazy dog"
	# Test 5
	wp_test_str "Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	rm -f expected output
}

if [[ $1 == "bonus" ]]; then
	whirlpool_test
fi
