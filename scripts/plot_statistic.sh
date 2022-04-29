reset
set xlabel 'F(n)'
set ylabel 'time (ns)'
set title 'Fibonacci runtime'
set term png enhanced font 'Verdana,10'
set output 'plot_statistic.png'
set grid
plot [0:92][:] \
'gnuplot_fibdrv_2.txt' using 1:2 with linespoints linewidth 2 title "fib\\_fast\\_doubling kernel",\
'gnuplot_fibdrv_3.txt' using 1:2 with linespoints linewidth 2 title "fib\\_fast\\_doubling\\_clz kernel"\
# '' using 1:3 with linespoints linewidth 2 title "fib\\_sequence user",\

# '' using 1:3 with linespoints linewidth 2 title "fib\\_fast\\_doubling user"