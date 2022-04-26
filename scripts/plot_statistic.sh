reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'plot_statistic.png'
set grid
plot [0:92][:] \
'gnuplot_fibdrv.txt' using 1:2 with linespoints linewidth 2 title "fib\\_sequence kernel",\
'' using 1:3 with linespoints linewidth 2 title "fib\\_sequence user"