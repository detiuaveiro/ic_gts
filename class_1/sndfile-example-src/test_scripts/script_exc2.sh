#!/bin/bash

# Check if the required command line argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <quantization bits>"
    exit 1
fi

# Extract the quantization bits from the argument
quantization_bits="$1"

# Define the fixed input and output file names
input_file="sample.wav"
output_file="outer.wav"

# Run wav_quant to perform quantization
../sndfile-example-bin/wav_quant "$input_file" "$quantization_bits" "$output_file"

# Check if wav_quant was successful
if [ $? -ne 0 ]; then
    echo "Error: wav_quant failed"
    exit 1
fi

# Write the number of quantization bits to exc2values.txt
echo "Quantization bits: $quantization_bits" >> exc2values.txt
# Run wav_cmp to compare the input and output files
../sndfile-example-bin/wav_cmp "$input_file" "$output_file" >> exc2values.txt

# Check if wav_cmp was successful
if [ $? -ne 0 ]; then
    echo "Error: wav_cmp failed"
    exit 1
fi

echo "Quantization and comparison completed. Results saved in exc2values.txt."
