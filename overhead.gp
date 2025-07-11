# Script Gnuplot para comparar o overhead de memória
set title "Overhead de Memória: Encadeado vs Indexado"
set xlabel "Tamanho do arquivo (blocos)"
set ylabel "Overhead (bytes)"
set key left top
set grid

# Ajuste dos eixos para melhor visualização
set xrange [0:600]
set yrange [0:2200]

# Configuração de saída em PNG
set terminal pngcairo size 800,600
set output 'overhead.png'

# Plotando os dados
plot "overhead.dat" using 1:2 with linespoints title "Encadeado", \
     "overhead.dat" using 1:3 with linespoints title "Indexado"
