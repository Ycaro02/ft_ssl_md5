#!/bin/bash

char_to_binary() {
    local char=$1
    printf "%08d" $(echo "obase=2; ibase=16; $(printf '%02X' "'$char")" | bc)
}

string_to_binary() {
    local string=$1
    local binary=""
    for (( i=0; i<${#string}; i++ )); do
        binary+=$(char_to_binary "${string:$i:1}")
        binary+=" "
    done
    echo $binary
}

# Check if the input is empty
if [ -z "$1" ]; then
	echo "Usage: $0 <string>"
	exit 1
fi

input="$1"

output=$(string_to_binary "$input" | tr -d ' ')
echo "$output"

