#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 input_file output_file"
    exit 1
fi

input_file="$1"
output_file="$2"

# Check if the input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: Input file '$input_file' not found."
    exit 1
fi

dos2unix ${input_file}

# Find the length of the longest line in the input file
max_length=$(awk '{ if (length > max) max = length } END { print max }' "$input_file")

# Function to center a line
center_line() {
    local line="$1"
    local line_length=${#line}
    local padding=$(( (max_length - line_length) / 2 ))
    printf "%${padding}s%s\n" " " "$line"
}

# Read input file line by line, center each line, and write to the output file
>"$output_file"
while IFS= read -r line; do
    centered_line=$(center_line "$line")
    echo "$centered_line" >> "$output_file"
done < "$input_file"

echo "Centred lines written to '$output_file'."
