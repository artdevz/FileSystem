set terminal pngcairo size 1920,1080 enhanced font 'Arial,18'
set output 'plots/benchmark_access.png'
set title 'Benchmark de Acesso Aleatório - Inode vs Linked'
set xlabel "Índice de bloco acessado"
set ylabel "Tempo em µs"
set yrange [0:2]
plot \
    "data/benchmark_access.png_data.txt" using 1:2 with points lt rgb 'blue' lw 2 title "Inode", \
    "data/benchmark_access.png_data.txt" using 1:3 with points lt rgb 'red' lw 2 title "Linked"
