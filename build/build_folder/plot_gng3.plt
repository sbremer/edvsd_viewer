set xlabel "X-Coordinate"
set ylabel "Y-Coordinate"
set zlabel "Orientation (atan/2)" offset 1,1,1

splot 'data.dat' using 1:2:3 with points title 'Data', 'gng.dat' using 1:2:3 with points title 'GNG', 'gnge.dat' every 1:1 using 1:2:3 with lines title 'GNG Edges'
