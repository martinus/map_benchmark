#!/bin/env gnuplot
reset

# png
set terminal pngcairo size 882,650 enhanced font 'Verdana,10'
set output 'scatterplot.png'

#set border linewidth 1.5
# Set first two line styles to blue (#0060ad) and red (#dd181f)

unset key

set xzeroaxis
set xlabel "total runtime [sec]"
set ylabel "x per second"
set title font 'Verdana,16' "best maps"
set rmargin 8
set tmargin 4

set yrange [0:*]
set xrange [0:*]

set style line 12 lc rgb'#808080' lt 0 lw 1
set grid back ls 12

#set format y "%.0s"

plot 'InsertHugeInt.dat' using 2:1:4:3 with labels center font "Verdana,8" point pt 7 ps 2 lc rgb variable offset char 0.0,0.0 notitle

