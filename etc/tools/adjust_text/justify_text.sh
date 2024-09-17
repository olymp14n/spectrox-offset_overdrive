#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 input_file output_file line_width"
    exit 1
fi

input_file="$1"
output_file="$2"
line_width="$3"

# Check if the input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found."
    exit 1
fi

dos2unix ${input_file}

cat "${input_file}" | par w${line_width} j1 | tee ${output_file}

echo "Justified lines written to '$output_file'."
