echo "args: $@"

out_dir="$1"
shift  # Pop out_dir.

# Process each remaining argument.
for arg in "$@"; do
    # Remove .in extension to get output filename without path.
    input_path="$arg"
    input_file="${input_path##*/}"
    output_file="${input_file%.in}"

    output_path="${out_dir}/${output_file}"

    # Replace @ZLIB_SYMBOL_PREFIX@ with empty string and write to new file.
    sed 's/@ZLIB_SYMBOL_PREFIX@//g' "$input_path" > "$output_path"
done
