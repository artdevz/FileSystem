set terminal pngcairo size 1920,1080 enhanced font 'Arial,18'
set output 'plots/overhead_comparison.png'
set title 'Overhead de Metadados - Inode vs Lista Encadeada'
set xlabel "Tamanho do arquivo (blocos)"
set ylabel "Overhead (bytes)"
set title "Overhead de Metadados - Inode vs Lista Encadeada"
set yrange [0:2000]
plot \
    "data/overhead_comparison.png_data.txt" using 1:2 with points lt rgb 'blue' lw 2 title "Inode Overhead", \
    "data/overhead_comparison.png_data.txt" using 1:3 with points lt rgb 'red' lw 2 title "Linked Overhead"
