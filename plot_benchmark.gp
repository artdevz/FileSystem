set datafile separator ","
set title "Benchmark de Acesso Aleatório - Inode vs Linked"
set xlabel "Índice do Bloco Acessado"
set ylabel "Tempo de Acesso (μs)"
set grid
set key outside

set term pngcairo size 1000,600 enhanced font 'Verdana,10'
set output "benchmark_plot.png"

plot "benchmark.csv" using 1:2 with points pointtype 7 linecolor rgb "blue" title "Inode", \
     "benchmark.csv" using 1:3 with points pointtype 7 linecolor rgb "red" title "Linked"
