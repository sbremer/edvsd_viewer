fx(x) = a1 * x**2 + b1 * x + c1
a1 = -1e-8; b1 = 1e-6; c1 = 30;

fy(x) = a2 * x**2 + b2 * x + c2
a2 = -1e-8; b2 = 1e-6; c2 = 30;

fit fx(x) 'data.dat' using 1:2 via a1, b1, c1
fit fy(x) 'data.dat' using 1:3 via a2, b2, c2

#plot 'data.dat' using 1:2 title 'X', fx(x), 'data.dat' using 1:3 title 'Y', fy(x)

#plot 'data.dat' using 1:2 title 'X', 'data.dat' using 1:3 title 'Y', 'data2.dat' using 1:2 title 'X_NN', 'data2.dat' using 1:3 title 'Y_NN'
