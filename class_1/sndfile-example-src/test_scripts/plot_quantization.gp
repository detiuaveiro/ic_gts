set term png
set output 'quantization_bar_plot.png'
set title 'Quantization of audio samples'
set xlabel 'Quantized Value'
set ylabel 'Count'
set style data boxes
set style fill solid 0.5
set boxwidth 0.8 relative

# Adjust the x-axis range
set xrange [-40000:40000]

plot 'quantized_samples.txt' using 1:(1) smooth freq with boxes title 'Quantized Values'

