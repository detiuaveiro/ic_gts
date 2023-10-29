#!/bin/bash

# Define the paths to the encoder and decoder executables
ENCODER="../sndfile-example-bin/encoder"
DECODER="../sndfile-example-bin/decoder"

# Define input and output file paths
INPUT_FILE="zeros_ones.txt"
BIN_FILE="encoded.bin"

# Run the encoder with the specified input and output files
$ENCODER "$INPUT_FILE" "$BIN_FILE"

# Check the exit status of the encoder
if [ $? -eq 0 ]; then
    echo "Encoder completed successfully. Now running the decoder..."
    
    # Run the decoder with the same output file as input
    $DECODER "$BIN_FILE" "decoded.txt"

    # Check the exit status of the decoder
    if [ $? -eq 0 ]; then
        echo "Decoder completed successfully."
    else
        echo "Decoder encountered an error."
    fi
else
    echo "Encoder encountered an error."
fi
